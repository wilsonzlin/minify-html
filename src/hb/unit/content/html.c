#include <hb/proc.h>
#include <hb/rune.h>

// Ensure COMMENT, BANG, and OPENING_TAG are together, and update _state_is_cbot if values are changed.
typedef enum {
    _STATE_COMMENT,
    _STATE_BANG,
    _STATE_OPENING_TAG,

    _STATE_END,
    _STATE_ENTITY,
    _STATE_TEXT,
} _state;

static bool _state_is_cbot(_state state) {
    return state >= _STATE_COMMENT &&
           state <= _STATE_OPENING_TAG;
}

static _state _get_next_state(hb_proc *proc) {
    hb_eof_rune c = hb_proc_peek_eof(proc);

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
 * There are three potential minification settings that affect whitespace handling:
 *   - collapse
 *   - destroy whole
 *   - trim
 * What whitespace to minify depends on the parent and configured settings.
 * We want to prevent memory allocation and use only one pass, but whitespace handling often involves looking ahead.
 */
void hb_unit_content_html(hb_proc* proc, nh_view_str* parent) {
    bool is_first_char = true;
    // Set to true when $whitespace is instantiated when $is_first_char is true
    bool whitespace_at_beginning = false;

    // Set to true after calling hb_unit_{comment|bang|tag}
    bool returned_from_comment_bang_or_tag = false;
    bool whitespace_after_right_chevron = false;
}
