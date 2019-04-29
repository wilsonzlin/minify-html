#include <hb/proc.h>
#include <hb/rule.h>
#include <hb/rune.h>
#include <hb/unit.h>

// Ensure COMMENT, BANG, and OPENING_TAG are together, and update _state_is_cbot
// if values are changed.
typedef enum {
	_STATE_COMMENT,
	_STATE_BANG,
	_STATE_OPENING_TAG,

	_STATE_START,
	_STATE_END,
	_STATE_ENTITY,
	_STATE_WHITESPACE,
	_STATE_TEXT,
} _state;

static bool _state_is_cbot(_state state)
{
	return state >= _STATE_COMMENT && state <= _STATE_OPENING_TAG;
}

static _state _get_next_state(hb_proc* proc)
{
	hb_eof_rune c = hb_proc_peek_eof(proc);

	if (c != HB_EOF && hb_rule_ascii_whitespace_check(c)) {
		return _STATE_WHITESPACE;
	}

	if (c == HB_EOF || hb_proc_matches(proc, "</")) {
		return _STATE_END;
	}

	if (hb_proc_matches(proc, "<!--")) {
		return _STATE_COMMENT;
	}

	// Check after comment
	if (hb_proc_matches(proc, "<!")) {
		return _STATE_BANG;
	}

	// Check after comment and bang
	if (c == '<') {
		return _STATE_OPENING_TAG;
	}

	if (c == '&') {
		return _STATE_ENTITY;
	}

	return _STATE_TEXT;
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
void hb_unit_content_html(hb_proc* proc, nh_view_str* parent)
{
	bool should_collapse_whitespace =
		hb_cfg_should_min(&proc->cfg->collapse_whitespace, parent);
	bool should_destroy_whole_whitespace =
		hb_cfg_should_min(&proc->cfg->destroy_whole_whitespace, parent);
	bool should_trim_whitespace =
		hb_cfg_should_min(&proc->cfg->trim_whitespace, parent);

	// Trim leading whitespace if configured to do so.
	if (should_trim_whitespace) {
		hb_proc_skip_while_predicate(proc,
					     &hb_rule_ascii_whitespace_check);
	}

	_state last_state = _STATE_START;
	hb_proc_view_init_src(whitespace, proc);
	// Whether or not currently in whitespace.
	bool whitespace_buffered = false;
	// If currently in whitespace, whether or not current contiguous
	// whitespace started after a bang, comment, or tag.
	bool whitespace_started_after_cbot = false;

	while (true) {
		_state next_state = _get_next_state(proc);

		if (next_state == _STATE_WHITESPACE) {
			// Whitespace is always buffered and then processed
			// afterwards, even if not minifying.
			hb_proc_skip(proc);

			if (last_state != _STATE_WHITESPACE) {
				// This is the start of one or more whitespace
				// characters, so start a view of this
				// contiguous whitespace and don't write any
				// characters that are part of it yet.
				hb_proc_view_start_with_src_next(&whitespace,
								 proc);
				whitespace_buffered = true;
				whitespace_started_after_cbot =
					_state_is_cbot(last_state);
			} else {
				// This is part of a contiguous whitespace, but
				// not the start of, so simply ignore.
			}

		} else {
			// Next character is not whitespace, so handle any
			// previously buffered whitespace.
			if (whitespace_buffered) {
				// Mark the end of the whitespace.
				hb_proc_view_end_with_src_prev(&whitespace,
							       proc);

				if (should_destroy_whole_whitespace
				    && whitespace_started_after_cbot
				    && _state_is_cbot(next_state)) {
					// Whitespace is between two tags,
					// comments, or bangs.
					// destroy_whole_whitespace is on, so
					// don't write it.

				} else if (should_trim_whitespace
					   && next_state == _STATE_END) {
					// Whitespace is trailing.
					// should_trim_whitespace is on, so
					// don't write it.

				} else if (should_collapse_whitespace) {
					// Current contiguous whitespace needs
					// to be reduced to a single space
					// character.
					hb_proc_write(proc, ' ');

				} else {
					// Whitespace cannot be minified, so
					// write in entirety.
					hb_proc_write_view(proc, &whitespace);
				}

				// Reset whitespace buffer.
				whitespace_buffered = false;
			}

			// Process and consume next character(s).
			switch (next_state) {
			case _STATE_COMMENT:
				hb_unit_comment(proc);
				break;

			case _STATE_BANG:
				hb_unit_bang(proc);
				break;

			case _STATE_OPENING_TAG:
				hb_unit_tag(proc, parent);
				break;

			case _STATE_END:
				break;

			case _STATE_ENTITY:
				hb_unit_entity(proc);
				break;

			case _STATE_TEXT:
				hb_proc_accept(proc);
				break;

			default:
				// Defensive coding.
				hb_proc_error(
					proc,
					HB_ERR_INTERR_UNKNOWN_CONTENT_NEXT_STATE,
					"Unknown content type");
			}
		}

		last_state = next_state;
		if (next_state == _STATE_END) {
			break;
		}
	}
}
