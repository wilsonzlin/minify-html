use crate::err::ProcessingResult;
use crate::proc::{Processor, ProcessorRange, UnintentionalEntityPrevention};
use crate::spec::codepoint::is_whitespace;
use crate::spec::tag::omission::CLOSING_TAG_OMISSION_RULES;
use crate::unit::bang::process_bang;
use crate::unit::comment::process_comment;
use crate::unit::entity::{EntityType, parse_entity};
use crate::unit::instruction::process_instruction;
use crate::unit::tag::{process_tag, ProcessedTag};
use crate::spec::tag::whitespace::{get_whitespace_minification_for_tag, WhitespaceMinification};

#[derive(Copy, Clone, PartialEq, Eq)]
enum ContentType {
    Comment,
    Bang,
    Instruction,
    OpeningTag,

    Start,
    End,
    Entity,
    Whitespace,
    Text,
}

impl ContentType {
    fn is_comment_bang_instruction_opening_tag(&self) -> bool {
        match self {
            ContentType::Comment | ContentType::Bang | ContentType::Instruction | ContentType::OpeningTag => true,
            _ => false,
        }
    }

    fn peek(proc: &mut Processor) -> ContentType {
        // Manually write out matching for fast performance as this is hot spot; don't use generated trie.
        match proc.peek_offset_eof(0) {
            None => ContentType::End,
            Some(b'<') => match proc.peek_offset_eof(1) {
                Some(b'/') => ContentType::End,
                Some(b'?') => ContentType::Instruction,
                Some(b'!') => match proc.peek_slice_offset_eof(2, 2) {
                    Some(b"--") => ContentType::Comment,
                    _ => ContentType::Bang,
                },
                _ => ContentType::OpeningTag
            },
            Some(b'&') => ContentType::Entity,
            Some(c) => if is_whitespace(c) { ContentType::Whitespace } else { ContentType::Text },
        }
    }
}

macro_rules! handle_content_type {
    ($proc:ident, $parent:ident, $next_content_type:expr, $uep:ident, $prev_sibling_closing_tag:ident, $get_entity:expr, $on_whitespace:block) => {
        // Process and consume next character(s).
        let next_content_type = $next_content_type;
        match next_content_type {
            ContentType::OpeningTag | ContentType::End | ContentType::Comment | ContentType::Bang | ContentType::Instruction => {
                // TODO Comment: Do not always initialise `uep` as `prev_sibling_closing_tag` might get written.
                $uep.take().map(|mut uep| $proc.after_write(&mut uep, true));
            }
            _ => {}
        };
        match next_content_type {
            ContentType::OpeningTag => {
                $prev_sibling_closing_tag = Some(process_tag($proc, $prev_sibling_closing_tag)?);
            }
            ContentType::End => {
                if let Some(prev_tag) = $prev_sibling_closing_tag {
                    let can_omit = match ($parent, CLOSING_TAG_OMISSION_RULES.get(&$proc[prev_tag.name])) {
                        (Some(parent_range), Some(rule)) => rule.can_omit_as_last_node(&$proc[parent_range]),
                        _ => false,
                    };
                    if !can_omit {
                        prev_tag.write_closing_tag($proc);
                    };
                };
                break;
            }
            content_type => {
                // Immediate next sibling node is not an element, so write any immediate previous sibling element's closing tag.
                $prev_sibling_closing_tag.take().map(|tag| tag.write_closing_tag($proc));
                match content_type {
                    ContentType::Comment | ContentType::Bang | ContentType::Instruction => {
                        match content_type {
                            ContentType::Comment => { process_comment($proc)?; }
                            ContentType::Bang => { process_bang($proc)?; }
                            ContentType::Instruction => { process_instruction($proc)?; }
                            _ => unreachable!(),
                        };
                    }
                    ContentType::Entity | ContentType::Text | ContentType::Whitespace => {
                        if $uep.is_none() {
                            $uep = Some($proc.start_preventing_unintentional_entities());
                        };
                        match content_type {
                            ContentType::Entity => {
                                let entity = $get_entity;
                                match entity {
                                    // TODO Comment: Explain why < is handled this way.
                                    EntityType::NonDecodableRightChevron(_) => $proc.after_write(&mut $uep.take().unwrap(), true),
                                    _ => {}
                                };
                                entity.keep($proc);
                            }
                            ContentType::Text => { $proc.accept()?; }
                            ContentType::Whitespace => $on_whitespace,
                            _ => unreachable!(),
                        };
                        // UEP could have become None after matching EntityType::NonDecodableRightChevron.
                        if let Some(uep) = $uep.as_mut() {
                            $proc.after_write(uep, false);
                        };
                    }
                    _ => unreachable!(),
                };
            }
        };
    };
}

fn process_wss_content(proc: &mut Processor, parent: Option<ProcessorRange>) -> ProcessingResult<()> {
    let mut prev_sibling_closing_tag: Option<ProcessedTag> = None;
    let mut uep: Option<UnintentionalEntityPrevention> = None;
    loop {
        handle_content_type!(proc, parent, ContentType::peek(proc), uep, prev_sibling_closing_tag, parse_entity(proc, false)?, { proc.accept()?; });
    };
    Ok(())
}

pub fn process_content(proc: &mut Processor, parent: Option<ProcessorRange>) -> ProcessingResult<()> {
    let &WhitespaceMinification { collapse, destroy_whole, trim } = get_whitespace_minification_for_tag(parent.map(|r| &proc[r]));

    if !(collapse || destroy_whole || trim) {
        // Normally whitespace entities are decoded and then ignored.
        // However, if whitespace cannot be minified in any way,
        // and we can't actually do anything but write whitespace as is,
        // we would have to simply write skipped whitespace. This would cause
        // issues when skipped whitespace includes encoded entities, so use
        // function that does no whitespace handling. It's probably faster too.
        return process_wss_content(proc, parent);
    };

    let mut last_non_whitespace_content_type = ContentType::Start;
    // Whether or not currently in whitespace.
    let mut currently_in_whitespace = false;
    // TODO Comment.
    let mut entity: Option<EntityType> = None;
    // TODO Comment.
    let mut prev_sibling_closing_tag: Option<ProcessedTag> = None;
    // TODO Comment.
    let mut uep: Option<UnintentionalEntityPrevention> = None;

    loop {
        let next_content_type = match ContentType::peek(proc) {
            ContentType::Entity => {
                // Entity could decode to whitespace.
                entity = Some(parse_entity(proc, false)?);
                let ws = match entity {
                    Some(EntityType::Ascii(c)) => is_whitespace(c),
                    _ => false,
                };
                if ws {
                    // Skip whitespace char, and mark as whitespace.
                    ContentType::Whitespace
                } else {
                    // Not whitespace, but don't write yet until any previously ignored whitespace has been processed later.
                    ContentType::Entity
                }
            }
            ContentType::Whitespace => {
                // Whitespace is always ignored and then processed afterwards, even if not minifying.
                proc.skip_expect();
                ContentType::Whitespace
            }
            other_type => other_type,
        };

        if next_content_type == ContentType::Whitespace {
            if !currently_in_whitespace {
                // This is the start of one or more whitespace characters.
                currently_in_whitespace = true;
            } else {
                // This is part of a contiguous whitespace, but not the start of, so simply ignore.
            }
            continue;
        }

        // Next character is not whitespace, so handle any previously ignored whitespace.
        if currently_in_whitespace {
            if destroy_whole && last_non_whitespace_content_type.is_comment_bang_instruction_opening_tag() && next_content_type.is_comment_bang_instruction_opening_tag() {
                // Whitespace is between two tags, comments, or bangs.
                // `destroy_whole` is on, so don't write it.
            } else if trim && (last_non_whitespace_content_type == ContentType::Start || next_content_type == ContentType::End) {
                // Whitespace is leading or trailing.
                // `trim` is on, so don't write it.
            } else if collapse {
                // Current contiguous whitespace needs to be reduced to a single space character.
                proc.write(b' ');
                // If writing space, then prev_sibling_closing_tag no longer represents immediate previous sibling node.
                prev_sibling_closing_tag.take().map(|tag| tag.write_closing_tag(proc));
            } else {
                unreachable!();
            };

            // Reset whitespace marker.
            currently_in_whitespace = false;
        };

        // Process and consume next character(s).
        handle_content_type!(proc, parent, next_content_type, uep, prev_sibling_closing_tag, entity.unwrap(), { unreachable!(); });
        last_non_whitespace_content_type = next_content_type;
    };

    Ok(())
}
