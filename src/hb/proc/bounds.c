#pragma once

#include <stdbool.h>
#include <hb/rune.h>
#include <hb/proc.h>

/**
 * Ensures that the provided character is not @{link HB_EOF}, and causes an error otherwise.
 *
 * @param c character to test for <code>HB_EOF</code>
 * @throws HBE_PARSE_UNEXPECTED_END if <code>c</code> is <code>HB_EOF</code>
 */
void hb_proc_bounds_assert_not_eof(hb_proc* proc, hb_eof_rune c) {
    if (c == HB_EOF) {
        hb_proc_error(proc, HB_ERR_PARSE_UNEXPECTED_END, "Unexpected end of input");
    }
}

/**
 * Checks that the <i>n</i>th character from current is less than or equal to length of source, where <i>n</i> is <code>offset</code>.
 * When <code>offset</code> is 1, it represents the next unconsumed character.
 *
 * @param proc proc
 * @param offset
 * @return true if src_next + offset <= src_len
 */
bool hb_proc_bounds_check_offset(hb_proc* proc, size_t offset) {
    return proc->src_next + offset <= proc->src_len;
}

/**
 * Asserts that the <i>n</i>th character from current is less than or equal to length of source, where <i>n</i> is <code>offset</code>.
 * When <code>offset</code> is 1, it represents the next unconsumed character.
 *
 * @param proc proc
 * @param offset
 * @throws HBE_PARSE_UNEXPECTED_END if <code>offset</code> exceeds end
 */
void hb_proc_bounds_assert_offset(hb_proc* proc, size_t offset) {
    if (!hb_proc_bounds_check_offset(proc, offset)) {
        hb_proc_error(proc, HB_ERR_PARSE_UNEXPECTED_END, "Unexpected end of input");
    }
}
