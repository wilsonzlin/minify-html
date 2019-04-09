#pragma once

#include <string.h>
#include <hb/proc.h>

/**
 * Checks if the next sequence of characters matches the character array <code>match</code>.
 * Won't cause an error if insufficient amount of characters left.
 *
 * @param proc proc
 * @param characters to check against
 * @return amount of characters matched, which should be equal to <code>strlen(match)</code>
 */
size_t hb_proc_matches_len(hb_proc* proc, char const* match, size_t match_len) {
    // Check that there are enough characters left.
    if (!hb_proc_bounds_check_offset(proc, match_len)) return 0;

    // Compare characters with fast memcmp.
    if (memcmp(&proc->src[proc->src_next], match, match_len) != 0) return 0;

    // Return amount of characters matched.
    return match_len;
}

/**
 * Checks if the next sequence of characters matches the character array <code>match</code> of lowercase characters ignoring case.
 * Won't cause an error if insufficient amount of characters left.
 *
 * @param proc proc
 * @param characters to check against ignoring case
 * @return amount of characters matched, which should be equal to <code>strlen(match)</code>
 */
size_t hb_proc_matches_len_i(hb_proc* proc, char const* match, size_t match_len) {
    // Check that there are enough characters left.
    if (!hb_proc_bounds_check_offset(proc, match_len)) return 0;

    // Compare characters ignoring case using strncasecmp.
    if (strncasecmp(&proc->src[proc->src_next], match, match_len) != 0) return 0;

    return match_len;
}

/**
 * Checks if the next sequence of characters is "\r", "\n", or "\r\n".
 * Won't cause an error if insufficient amount of characters left.
 *
 * @param proc proc
 * @return amount of characters matched
 */
size_t hb_proc_matches_line_terminator(hb_proc* proc) {
    // Comparing against `\r\n` must be done before `\r`.
    return hb_proc_matches(proc, "\r\n") ? 2 :
        hb_proc_matches(proc, "\r") ? 1 :
        hb_proc_matches(proc, "\n");
}
