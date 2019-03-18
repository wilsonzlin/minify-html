#pragma once

#include <hb/proc.h>

/**
 * Checks if the next sequence of characters matches the character array <code>match</code>.
 * Won't cause an error if insufficient amount of characters left.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param match characters to match
 * @return amount of characters matched, which should be equal to <code>strlen(match)</code>
 * @throws on read error
 */
size_t hb_proc_matches_len(hb_proc* proc, char const* match, size_t match_len) {
    if (proc->src_next + match_len > proc->src_len) return 0;

    for (size_t i = 0; i < match_len; i++) {
        if (proc->src[proc->src_next + i] != match[i]) return 0;
    }

    return match_len;
}
