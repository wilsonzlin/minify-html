#pragma once

#include <stddef.h>
#include <hb/rune.h>
#include <hb/proc.h>

/**
 * Gets the next character.
 * If all characters have already been consumed, {@link HB_EOF} is returned.
 *
 * @param proc proc
 * @return character or {@link HB_EOF}
 */
hb_eof_rune hb_proc_peek_eof(hb_proc* proc) {
    return proc->src[proc->src_next];
}

/**
 * Gets the next character.
 * Will cause an error if it's the end and there is no next character.
 *
 * @param proc proc
 * @return character
 * @throws on HBE_PARSE_UNEXPECTED_END
 */
hb_rune hb_proc_peek(hb_proc* proc) {
    hb_eof_rune c = hb_proc_peek_eof(proc);

    hb_proc_bounds_assert_not_eof(proc, c);

    return c;
}

/**
 * Gets the <i>n</i>th character from current, where <i>n</i> is <code>offset</code>.
 * When <code>offset</code> is 1, the next character is returned (equivalent to {@link hb_proc_peek_eof}).
 * If <code>offset</code> is after the last character, {@link HB_EOF} is returned.
 *
 * @param proc proc
 * @param offset position of character to get
 * @return character or {@link HB_EOF}
 */
hb_eof_rune hb_proc_peek_eof_offset(hb_proc* proc, size_t offset) {
    if (!hb_proc_bounds_check_offset(proc, offset)) return HB_EOF;

    return proc->src[proc->src_next + offset];
}

/**
 * Gets the <i>n</i>th character from current, where <i>n</i> is <code>offset</code>.
 * When <code>offset</code> is 1, the next character is returned (equivalent to {@link hb_proc_peek_offset}).
 * An error will be caused if <code>offset</code> is after the last character.
 *
 * @param proc proc
 * @param offset position of character to get
 * @return character
 * @throws on HBE_PARSE_UNEXPECTED_END
 */
hb_rune hb_proc_peek_offset(hb_proc* proc, size_t offset) {
    hb_eof_rune c = hb_proc_peek_eof_offset(proc, offset);

    hb_proc_bounds_assert_not_eof(proc, c);

    return c;
}
