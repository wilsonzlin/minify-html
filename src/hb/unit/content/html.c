#pragma once

#include <hb/proc.h>
#include <hb/rune.h>

// Ensure COMMENT, BANG, and OPENING_TAG are together, and update _state_is_cbot if values changed
typedef enum {
    HB_UNIT_CONTENT_HTML_STATE_COMMENT = 1,
    HB_UNIT_CONTENT_HTML_STATE_BANG,
    HB_UNIT_CONTENT_HTML_STATE_OPENING_TAG = 3,
    HB_UNIT_CONTENT_HTML_STATE_END,
    HB_UNIT_CONTENT_HTML_STATE_ENTITY,
    HB_UNIT_CONTENT_HTML_STATE_TEXT,
} hb_unit_content_html_state;

static bool _state_is_cbot(hb_unit_content_html_state state) {
    return state >= HB_UNIT_CONTENT_HTML_STATE_COMMENT &&
           state <= HB_UNIT_CONTENT_HTML_STATE_OPENING_TAG;
}

static hb_unit_content_html_state _get_next_state(hb_proc *proc) {
    hb_rune c = hb_proc_peek_eof(proc);

    if (c == HB_EOF || hb_proc_matches(proc, "</")) {
        return HB_UNIT_CONTENT_HTML_STATE_END;
    }

    if (hb_proc_matches(proc, "<!--")) {
        return HB_UNIT_CONTENT_HTML_STATE_COMMENT;
    }

    // Check after comment
    if (hb_proc_matches(proc, "<!")) {
        return HB_UNIT_CONTENT_HTML_STATE_BANG;
    }

    // Check after comment and bang
    if (c == '<') {
        return HB_UNIT_CONTENT_HTML_STATE_OPENING_TAG;
    }

    if (c == '&') {
        return HB_UNIT_CONTENT_HTML_STATE_ENTITY;
    }

    return HB_UNIT_CONTENT_HTML_STATE_TEXT;
}

/*
 * Whitespace handling is the trickiest part of this function.
 * There are three potential minification settings that affect whitespace handling:
 *   - collapse
 *   - destroy whole
 *   - trim
 * The types of whitespace minification depends on the parent and configured settings.
 * We want to prevent memory allocation and use only one pass, but whitespace handling often involves looking ahead.
 */
void hb_unit_content_html(hb_proc* proc, hb_proc_view parent) {
    bool is_first_char = true;
    // Set to true when $whitespace is instantiated when $is_first_char is true
    bool whitespace_at_beginning = false;

    // Set to true after calling hb_unit_{comment|bang|tag}
    bool returned_from_comment_bang_or_tag = false;
    bool whitespace_after_right_chevron = false;
}
