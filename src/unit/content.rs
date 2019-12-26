use crate::err::ProcessingResult;
use crate::proc::{Checkpoint, Processor, ProcessorRange};
use crate::spec::codepoint::is_whitespace;
use crate::spec::tag::content::CONTENT_TAGS;
use crate::spec::tag::formatting::FORMATTING_TAGS;
use crate::spec::tag::wss::WSS_TAGS;
use crate::unit::bang::process_bang;
use crate::unit::comment::process_comment;
use crate::unit::entity::{process_entity, maybe_process_entity};
use crate::unit::tag::process_tag;

#[derive(Copy, Clone, PartialEq, Eq, Debug)]
enum ContentType {
    Comment,
    Bang,
    OpeningTag,

    Start,
    End,
    Entity,
    Whitespace,
    Text,
}

impl ContentType {
    fn is_comment_bang_opening_tag(&self) -> bool {
        match self {
            ContentType::Comment | ContentType::Bang | ContentType::OpeningTag => true,
            _ => false,
        }
    }

    fn peek(proc: &mut Processor) -> ContentType {
        // TODO Optimise to trie.

        if proc.at_end() || chain!(proc.match_seq(b"</").matched()) {
            return ContentType::End;
        };

        if chain!(proc.match_pred(is_whitespace).matched()) {
            return ContentType::Whitespace;
        };

        if chain!(proc.match_seq(b"<!--").matched()) {
            return ContentType::Comment;
        };

        // Check after comment
        if chain!(proc.match_seq(b"<!").matched()) {
            return ContentType::Bang;
        };

        // Check after comment and bang
        if chain!(proc.match_char(b'<').matched()) {
            return ContentType::OpeningTag;
        };

        if chain!(proc.match_char(b'&').matched()) {
            return ContentType::Entity;
        };

        ContentType::Text
    }
}

pub fn process_content(proc: &mut Processor, parent: Option<ProcessorRange>) -> ProcessingResult<()> {
    let should_collapse_whitespace = match parent {
        Some(tag_name) => !WSS_TAGS.contains(&proc[tag_name]),
        // Should collapse whitespace for root content.
        None => true,
    };
    let should_destroy_whole_whitespace = match parent {
        Some(tag_name) => !WSS_TAGS.contains(&proc[tag_name]) && !CONTENT_TAGS.contains(&proc[tag_name]) && !FORMATTING_TAGS.contains(&proc[tag_name]),
        // Should destroy whole whitespace for root content.
        None => true,
    };
    let should_trim_whitespace = match parent {
        Some(tag_name) => !WSS_TAGS.contains(&proc[tag_name]) && !FORMATTING_TAGS.contains(&proc[tag_name]),
        // Should trim whitespace for root content.
        None => true,
    };

    // Trim leading whitespace if configured to do so.
    if should_trim_whitespace {
        chain!(proc.match_while_pred(is_whitespace).discard());
    };

    let mut last_non_whitespace_content_type = ContentType::Start;
    // Whether or not currently in whitespace.
    let mut whitespace_checkpoint_opt: Option<Checkpoint> = None;

    loop {
        let next_content_type = match ContentType::peek(proc) {
            ContentType::Entity => {
                let e = maybe_process_entity(proc)?;
                // Entity could decode to whitespace.
                if e.code_point()
                    .filter(|c| *c < 0x7f)
                    .filter(|c| is_whitespace(*c as u8))
                    .is_some() {
                    // Skip whitespace char, and mark as whitespace.
                    ContentType::Whitespace
                } else {
                    // Not whitespace, so decode and write.
                    e.keep(proc);
                    ContentType::Entity
                }
            },
            ContentType::Whitespace => {
                // This is here to prevent skipping twice from decoded whitespace entity.
                // Whitespace is always ignored and then processed afterwards, even if not minifying.
                proc.skip().expect("skipping known character");
                ContentType::Whitespace
            },
            other_type => other_type,
        };

        if next_content_type == ContentType::Whitespace {
            if let None = whitespace_checkpoint_opt {
                // This is the start of one or more whitespace characters, so start a view of this contiguous whitespace
                // and don't write any characters that are part of it yet.
                whitespace_checkpoint_opt = Some(proc.checkpoint());
            } else {
                // This is part of a contiguous whitespace, but not the start of, so simply ignore.
            }
            continue;
        }

        // Next character is not whitespace, so handle any previously ignored whitespace.
        if let Some(ws) = whitespace_checkpoint_opt {
            if should_destroy_whole_whitespace && last_non_whitespace_content_type.is_comment_bang_opening_tag() && next_content_type.is_comment_bang_opening_tag() {
                // Whitespace is between two tags, comments, or bangs.
                // destroy_whole_whitespace is on, so don't write it.
            } else if should_trim_whitespace && (next_content_type == ContentType::End || last_non_whitespace_content_type == ContentType::Start) {
                // Whitespace is leading or trailing.
                // should_trim_whitespace is on, so don't write it.
            } else if should_collapse_whitespace {
                // Current contiguous whitespace needs to be reduced to a single space character.
                proc.write(b' ');
            } else {
                // Whitespace cannot be minified, so write in entirety.
                proc.write_skipped(ws);
            }

            // Reset whitespace buffer.
            whitespace_checkpoint_opt = None;
        };

        // Process and consume next character(s).
        match next_content_type {
            ContentType::Comment => { process_comment(proc)?; }
            ContentType::Bang => { process_bang(proc)?; }
            ContentType::OpeningTag => { process_tag(proc)?; }
            ContentType::End => { break; }
            // Entity has already been processed.
            ContentType::Entity => {}
            ContentType::Text => { proc.accept()?; }
            _ => unreachable!(),
        };

        last_non_whitespace_content_type = next_content_type;
    };

    Ok(())
}
