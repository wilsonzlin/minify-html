use crate::code::Code;
use crate::proc::Processor;
use crate::spec::codepoint::is_whitespace;
use crate::proc::comment::process_comment;
use crate::proc::bang::process_bang;
use crate::proc::entity::process_entity;
use crate::proc::tag::process_tag;
use crate::err::HbRes;
use crate::spec::tag::wss::WSS_TAGS;
use crate::spec::tag::content::CONTENT_TAGS;
use crate::spec::tag::formatting::FORMATTING_TAGS;

#[derive(PartialEq)]
enum State {
	Comment,
	Bang,
	OpeningTag,

	Start,
	End,
	Entity,
	Whitespace,
	Text,
}

impl State {
	fn is_comment_bang_opening_tag(&self) -> bool {
		match self {
			State::Comment | State::Bang | State::OpeningTag => true,
			_ => false,
		}
	}

	fn next_state<D: Code>(proc: &Processor<D>) -> State {
		// TODO Optimise to trie.

		if proc.data.at_end() || proc.match_seq(b"</").matched() {
			return State::End;
		}

		if proc.match_pred(is_whitespace).matched() {
			return State::Whitespace;
		}

		if proc.match_seq(b"<!--").matched() {
			return State::Comment;
		}

		// Check after comment
		if proc.match_seq(b"<!").matched() {
			return State::Bang;
		};

		// Check after comment and bang
		if proc.match_char(b'<').matched() {
			return State::OpeningTag;
		};

		if proc.match_char(b'&').matched() {
			return State::Entity;
		};

		return State::Text;
	}
}

/*
 * Whitespace handling is the trickiest part of this function.
 * There are three potential minification settings that affect whitespace
 * handling:
 *   - collapse
 *   - destroy whole
 *   - trim
 * What whitespace to minify depends on the parent and configured settings.
 * We want to prevent memory allocation and use only one pass, but whitespace
 * handling often involves looking ahead.
 */
pub fn process_content<D: Code>(proc: &Processor<D>, parent: Option<&[u8]>) -> HbRes<()> {
	let should_collapse_whitespace = parent.filter(|p| !WSS_TAGS.contains(p)).is_some();
	let should_destroy_whole_whitespace = parent.filter(|p| !WSS_TAGS.contains(p) && !CONTENT_TAGS.contains(p) && !FORMATTING_TAGS.contains(p)).is_some();
	let should_trim_whitespace = parent.filter(|p| !WSS_TAGS.contains(p) && !FORMATTING_TAGS.contains(p)).is_some();

	// Trim leading whitespace if configured to do so.
	if should_trim_whitespace {
		proc.match_while_pred(is_whitespace).discard();
	};

	let mut last_state = State::Start;
	// Whether or not currently in whitespace.
	let mut whitespace_start = None;
	// If currently in whitespace, whether or not current contiguous
	// whitespace started after a bang, comment, or tag.
	let mut whitespace_started_after_cbot = false;

	loop {
		let next_state = State::next_state(proc);

		if next_state == State::Whitespace {
			// Whitespace is always buffered and then processed
			// afterwards, even if not minifying.
			proc.skip();

			if last_state != State::Whitespace {
				// This is the start of one or more whitespace
				// characters, so start a view of this
				// contiguous whitespace and don't write any
				// characters that are part of it yet.
				whitespace_start = Some(proc.start_read_slice());
				whitespace_started_after_cbot = last_state.is_comment_bang_opening_tag();
			} else {
				// This is part of a contiguous whitespace, but
				// not the start of, so simply ignore.
			}
		} else {
			// Next character is not whitespace, so handle any
			// previously buffered whitespace.
			if let Some(whitespace_buffered) = whitespace_start {
				if should_destroy_whole_whitespace && whitespace_started_after_cbot && next_state.is_comment_bang_opening_tag() {
					// Whitespace is between two tags, comments, or bangs.
					// destroy_whole_whitespace is on, so don't write it.
				} else if should_trim_whitespace && next_state == State::End {
					// Whitespace is trailing.
					// should_trim_whitespace is on, so don't write it.
				} else if should_collapse_whitespace {
					// Current contiguous whitespace needs to be reduced to a single space character.
					proc.write(b' ');
				} else {
					// Whitespace cannot be minified, so
					// write in entirety.
					proc.write_slice(proc.get_slice(whitespace_buffered));
				}

				// Reset whitespace buffer.
				whitespace_start = None;
			};

			// Process and consume next character(s).
			match next_state {
				State::Comment => process_comment(proc),
				State::Bang => process_bang(proc),
				State::OpeningTag => process_tag(proc, parent),
				State::End => (),
				State::Entity => process_entity(proc),
				State::Text => proc.accept(),
				_ => unreachable!(),
			};
		};

		last_state = next_state;
		if next_state == State::End {
			break;
		};
	};

	Ok(())
}
