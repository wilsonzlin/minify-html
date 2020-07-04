use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
use crate::proc::range::ProcessorRange;
use crate::spec::codepoint::is_whitespace;
use crate::spec::tag::omission::CLOSING_TAG_OMISSION_RULES;
use crate::spec::tag::whitespace::{get_whitespace_minification_for_tag, WhitespaceMinification};
use crate::unit::bang::process_bang;
use crate::unit::comment::process_comment;
use crate::unit::instruction::process_instruction;
use crate::unit::tag::{MaybeClosingTag, process_tag};
use crate::spec::tag::ns::Namespace;
use crate::proc::entity::maybe_normalise_entity;

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
    fn is_tag(&self) -> bool {
        match self {
            ContentType::Tag => true,
            _ => false,
        }
    }

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
                _ => ContentType::Tag
            },
            Some(_) => ContentType::Text,
        }
    }
}

pub fn process_content(proc: &mut Processor, ns: Namespace, parent: Option<ProcessorRange>) -> ProcessingResult<()> {
    let &WhitespaceMinification { collapse, destroy_whole, trim } = get_whitespace_minification_for_tag(parent.map(|r| &proc[r]));

    let handle_ws = collapse || destroy_whole || trim;

    let mut last_written = ContentType::Start;
    // Whether or not currently in whitespace.
    let mut ws_skipped = false;
    let mut prev_sibling_closing_tag = MaybeClosingTag::none();

    loop {
        // WARNING: Do not write anything until any previously ignored whitespace has been processed later.

        // Process comments, bangs, and instructions, which are completely ignored and do not affect anything (previous element node's closing tag, unintentional entities, whitespace, etc.).
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

        let next_is_decoded_chevron = maybe_normalise_entity(proc) && proc.peek(0).filter(|c| *c == b'<').is_some();

        if handle_ws {
            // If any of these arms match, this is the start or part of one or more whitespace characters.
            // Simply ignore and process until first non-whitespace.
            if match next_content_type {
                ContentType::Text => proc.m(IsPred(is_whitespace), Discard).nonempty(),
                _ => false,
            } {
                ws_skipped = true;
                continue;
            };

            // Next character is not whitespace, so handle any previously ignored whitespace.
            if ws_skipped {
                if destroy_whole && last_written.is_tag() && next_content_type.is_tag() {
                    // Whitespace is between two tags, instructions, or bangs.
                    // `destroy_whole` is on, so don't write it.
                } else if trim && (last_written == ContentType::Start || next_content_type == ContentType::End) {
                    // Whitespace is leading or trailing.
                    // `trim` is on, so don't write it.
                } else if collapse {
                    // If writing space, then prev_sibling_closing_tag no longer represents immediate previous sibling node; space will be new previous sibling node (as a text node).
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

        if next_is_decoded_chevron {
            // Problem: semicolon after encoded '<' will cause '&LT;', making it part of the entity.
            // Solution: insert another semicolon.
            let encoded: &[u8] = match proc.peek(1) {
                // Use "&LT" instead of "&lt" as there are other entity names starting with "lt".
                Some(b';') => b"&LT;",
                _ => b"&LT",
            };
            proc.write_slice(encoded);
            proc.skip_expect();
            continue;
        };

        // Process and consume next character(s).
        match next_content_type {
            ContentType::Tag => {
                let new_closing_tag = process_tag(proc, ns, prev_sibling_closing_tag)?;
                prev_sibling_closing_tag.replace(new_closing_tag);
            }
            ContentType::End => {
                if prev_sibling_closing_tag.exists_and(|prev_tag|
                    CLOSING_TAG_OMISSION_RULES
                        .get(&proc[prev_tag])
                        .filter(|rule| rule.can_omit_as_last_node(parent.map(|p| &proc[p])))
                        .is_none()
                ) {
                    prev_sibling_closing_tag.write(proc);
                };
                break;
            }
            ContentType::Text => {
                // Immediate next sibling node is not an element, so write any immediate previous sibling element's closing tag.
                if prev_sibling_closing_tag.exists() {
                    prev_sibling_closing_tag.write(proc);
                };
                proc.accept()?;
            }
            _ => unreachable!(),
        };

        // This should not be reached if ContentType::{Comment, End}.
        last_written = next_content_type;
    };

    Ok(())
}
