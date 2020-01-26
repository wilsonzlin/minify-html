use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
use crate::proc::range::ProcessorRange;
use crate::proc::uep::UnintentionalEntityPrevention;
use crate::spec::codepoint::is_whitespace;
use crate::spec::tag::omission::CLOSING_TAG_OMISSION_RULES;
use crate::spec::tag::whitespace::{get_whitespace_minification_for_tag, WhitespaceMinification};
use crate::unit::bang::process_bang;
use crate::unit::comment::process_comment;
use crate::unit::entity::{EntityType, parse_entity};
use crate::unit::instruction::process_instruction;
use crate::unit::tag::{MaybeClosingTag, Namespace, process_tag};

#[derive(Copy, Clone, PartialEq, Eq)]
enum ContentType {
    Comment,
    Bang,
    Instruction,
    Tag,

    Start,
    End,
    Entity,
    Text,
}

impl ContentType {
    fn is_tag_like(&self) -> bool {
        // Do not include Comment as comments are not written.
        match self {
            ContentType::Bang | ContentType::Instruction | ContentType::Tag => true,
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
            Some(b'&') => ContentType::Entity,
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
    // TODO Comment: Do not always initialise `uep` as `prev_sibling_closing_tag` might get written.
    let mut prev_sibling_closing_tag = MaybeClosingTag::none();
    // TODO Comment.
    let uep = &mut UnintentionalEntityPrevention::new(proc, true);

    loop {
        // Do not write anything until any previously ignored whitespace has been processed later.
        let next_content_type = ContentType::peek(proc);
        let entity: Option<EntityType> = match next_content_type {
            ContentType::Comment => {
                // Comments are completely ignored and do not affect anything (previous element node's closing tag, unintentional entities, whitespace, etc.).
                process_comment(proc)?;
                continue;
            }
            ContentType::Entity => Some(parse_entity(proc)?),
            _ => None,
        };

        if handle_ws {
            // If any of these arms match, this is the start or part of one or more whitespace characters.
            // Simply ignore and process until first non-whitespace.
            if match (next_content_type, entity) {
                (_, Some(EntityType::Ascii(c))) if is_whitespace(c) => true,
                (ContentType::Text, _) => proc.m(IsPred(is_whitespace), Discard).nonempty(),
                _ => false,
            } {
                ws_skipped = true;
                continue;
            };

            // Next character is not whitespace, so handle any previously ignored whitespace.
            if ws_skipped {
                if destroy_whole && last_written.is_tag_like() && next_content_type.is_tag_like() {
                    // Whitespace is between two tags, comments, instructions, or bangs.
                    // `destroy_whole` is on, so don't write it.
                } else if trim && (last_written == ContentType::Start || next_content_type == ContentType::End) {
                    // Whitespace is leading or trailing.
                    // `trim` is on, so don't write it.
                } else if collapse {
                    // If writing space, then prev_sibling_closing_tag no longer represents immediate previous sibling node; space will be new previous sibling node (as a text node).
                    uep_ignore!(uep, proc, {
                        prev_sibling_closing_tag.write_if_exists(proc);
                    });
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
                // Always resume UEP as closing tag might not exist or be omitted.
                uep_ignore!(uep, proc, {
                    let new_closing_tag = process_tag(proc, ns, prev_sibling_closing_tag)?;
                    prev_sibling_closing_tag.replace(new_closing_tag);
                });
            }
            ContentType::End => {
                uep.end(proc);
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
            content_type => {
                // Immediate next sibling node is not an element, so write any immediate previous sibling element's closing tag.
                // UEP is resumed after processing a tag and setting `prev_sibling_closing_tag` (see ContentType::Tag arm), so suspend it before writing any closing tag (even though nothing should've been written since tag was processed and `prev_sibling_closing_tag` was set).
                if prev_sibling_closing_tag.exists() {
                    uep_ignore!(uep, proc, {
                        prev_sibling_closing_tag.write(proc);
                    });
                };
                match content_type {
                    ContentType::Bang | ContentType::Instruction => uep_ignore!(uep, proc, {
                        match content_type {
                            ContentType::Bang => { process_bang(proc)?; }
                            ContentType::Instruction => { process_instruction(proc)?; }
                            _ => unreachable!(),
                        };
                    }),
                    ContentType::Entity | ContentType::Text => uep_process!(uep, proc, {
                        match entity {
                            Some(entity) => { entity.keep(proc); }
                            // Is text.
                            None => { proc.accept()?; }
                        };
                    }),
                    _ => unreachable!(),
                };
            }
        };

        // This should not be reached if ContentType::{Comment, End}.
        last_written = next_content_type;
    };

    Ok(())
}
