use crate::cfg::Cfg;
use crate::common::gen::codepoints::{TAG_NAME_CHAR, WHITESPACE};
use crate::common::spec::tag::ns::Namespace;
use crate::common::spec::tag::omission::{can_omit_as_before, can_omit_as_last_node};
use crate::common::spec::tag::whitespace::{
    get_whitespace_minification_for_tag, WhitespaceMinification,
};
use crate::err::ProcessingResult;
use crate::proc::checkpoint::ReadCheckpoint;
use crate::proc::entity::maybe_normalise_entity;
use crate::proc::range::ProcessorRange;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
use crate::unit::bang::process_bang;
use crate::unit::comment::process_comment;
use crate::unit::instruction::process_instruction;
use crate::unit::tag::{process_tag, MaybeClosingTag};

#[derive(Copy, Clone, PartialEq, Eq)]
enum ContentType {
    Comment,
    Bang,
    Instruction,
    Tag,

    Start,
    End,
    Text,
}

impl ContentType {
    fn peek(proc: &mut Processor) -> ContentType {
        // Manually write out matching for fast performance as this is hot spot; don't use generated trie.
        match proc.peek(0) {
            None => ContentType::End,
            Some(b'<') => match proc.peek(1) {
                Some(b'/') => ContentType::End,
                Some(b'?') => ContentType::Instruction,
                Some(b'!') => match proc.peek_many(2, 2) {
                    Some(b"--") => ContentType::Comment,
                    _ => ContentType::Bang,
                },
                Some(c) if TAG_NAME_CHAR[c] => ContentType::Tag,
                _ => ContentType::Text,
            },
            Some(_) => ContentType::Text,
        }
    }
}

pub struct ProcessedContent {
    pub closing_tag_omitted: bool,
}

pub fn process_content(
    proc: &mut Processor,
    cfg: &Cfg,
    ns: Namespace,
    parent: Option<ProcessorRange>,
    descendant_of_pre: bool,
) -> ProcessingResult<ProcessedContent> {
    let &WhitespaceMinification {
        collapse,
        destroy_whole,
        trim,
    } = get_whitespace_minification_for_tag(ns, proc.get_or_empty(parent), descendant_of_pre);

    let handle_ws = collapse || destroy_whole || trim;

    let mut last_written = ContentType::Start;
    // Whether or not currently in whitespace.
    let mut ws_skipped = false;
    let mut prev_sibling_closing_tag = MaybeClosingTag::none();

    loop {
        // WARNING: Do not write anything until any previously ignored whitespace has been processed later.

        // Process comments, bangs, and instructions, which are completely ignored and do not affect anything (previous
        // element node's closing tag, unintentional entities, whitespace, etc.).
        let next_content_type = ContentType::peek(proc);
        match next_content_type {
            ContentType::Comment => {
                process_comment(proc)?;
                continue;
            }
            ContentType::Bang => {
                process_bang(proc)?;
                continue;
            }
            ContentType::Instruction => {
                process_instruction(proc)?;
                continue;
            }
            _ => {}
        };

        maybe_normalise_entity(proc, false);

        if handle_ws {
            if next_content_type == ContentType::Text
                && proc.m(IsInLookup(WHITESPACE), Discard).nonempty()
            {
                // This is the start or part of one or more whitespace characters.
                // Simply ignore and process until first non-whitespace.
                ws_skipped = true;
                continue;
            };

            // Next character is not whitespace, so handle any previously ignored whitespace.
            if ws_skipped {
                if destroy_whole
                    && last_written == ContentType::Tag
                    && next_content_type == ContentType::Tag
                {
                    // Whitespace is between two tags, instructions, or bangs.
                    // `destroy_whole` is on, so don't write it.
                } else if trim
                    && (last_written == ContentType::Start || next_content_type == ContentType::End)
                {
                    // Whitespace is leading or trailing.
                    // `trim` is on, so don't write it.
                } else if collapse {
                    // If writing space, then prev_sibling_closing_tag no longer represents immediate previous sibling
                    // node; space will be new previous sibling node (as a text node).
                    prev_sibling_closing_tag.write_if_exists(proc);
                    // Current contiguous whitespace needs to be reduced to a single space character.
                    proc.write(b' ');
                    last_written = ContentType::Text;
                } else {
                    unreachable!();
                };

                // Reset whitespace marker.
                ws_skipped = false;
            };
        };

        // Process and consume next character(s).
        match next_content_type {
            ContentType::Tag => {
                let tag_checkpoint = ReadCheckpoint::new(proc);
                proc.skip_expect();
                let tag_name = proc
                    .m(WhileInLookup(TAG_NAME_CHAR), Discard)
                    .require("tag name")?;
                proc.make_lowercase(tag_name);

                if can_omit_as_before(proc.get_or_empty(parent), &proc[tag_name]) {
                    // TODO Is this necessary? Can a previous closing tag even exist?
                    prev_sibling_closing_tag.write_if_exists(proc);
                    tag_checkpoint.restore(proc);
                    return Ok(ProcessedContent {
                        closing_tag_omitted: true,
                    });
                };

                let new_closing_tag = process_tag(
                    proc,
                    cfg,
                    ns,
                    parent,
                    descendant_of_pre
                        || ns == Namespace::Html
                            && parent.filter(|p| &proc[*p] == b"pre").is_some(),
                    prev_sibling_closing_tag,
                    tag_name,
                )?;
                prev_sibling_closing_tag.replace(new_closing_tag);
            }
            ContentType::End => {
                if prev_sibling_closing_tag.exists_and(|prev_tag| {
                    !can_omit_as_last_node(proc.get_or_empty(parent), &proc[prev_tag])
                }) {
                    prev_sibling_closing_tag.write(proc);
                };
                break;
            }
            ContentType::Text => {
                // Immediate next sibling node is not an element, so write any immediate previous sibling element's closing tag.
                if prev_sibling_closing_tag.exists() {
                    prev_sibling_closing_tag.write(proc);
                };

                let c = proc.peek(0).unwrap();

                // From the spec: https://html.spec.whatwg.org/multipage/parsing.html#tag-open-state
                // After a `<`, a valid character is an ASCII alpha, `/`, `!`, or `?`. Anything
                // else, and the `<` is treated as content.
                if proc.last_is(b'<') && (TAG_NAME_CHAR[c] || c == b'?' || c == b'!' || c == b'/') {
                    // We need to encode the `<` that we just wrote as otherwise this char will
                    // cause it to be interpreted as something else (e.g. opening tag).
                    // NOTE: This conditional should mean that we never have to worry about a
                    // semicolon after encoded `<` becoming `&LT;` and part of the entity, as the
                    // only time `&LT` appears is when we write it here; every other time we always
                    // decode any encoded `<`.
                    // TODO Optimise, maybe using last written flag.
                    proc.undo_write(1);
                    // We use `LT` because no other named entity starts with it so it can't be
                    // misinterpreted as another entity or require a semicolon.
                    proc.write_slice(b"&LT");
                };

                proc.accept_expect();
            }
            _ => unreachable!(),
        };

        // This should not be reached if ContentType::{Comment, End}.
        last_written = next_content_type;
    }

    Ok(ProcessedContent {
        closing_tag_omitted: false,
    })
}
