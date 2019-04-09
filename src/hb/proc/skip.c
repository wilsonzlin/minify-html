#pragma once

#include <hb/rune.h>
#include <hb/proc.h>

/**
 * Skips over the next character.
 * Requires that the file has at least one character remaining.
 *
 * @param proc proc
 * @return skipped character
 * @throws on HBE_PARSE_UNEXPECTED_END
 */
hb_rune hb_proc_skip(hb_proc* proc) {
    hb_rune c = proc->src[proc->src_next];

    hb_proc_bounds_assert_not_eof(proc, c);

    proc->src_next++;

    return c;
}

/**
 * Skips over the next <code>amount</code> characters.
 * Requires that the file has at least <code>amount</code> characters remaining.
 *
 * @param proc proc
 * @param amount amount of characters to skip
 * @throws on HBE_PARSE_UNEXPECTED_END
 */
void hb_proc_skip_amount(hb_proc* proc, size_t amount) {
    hb_proc_bounds_assert_offset(proc, amount);

    proc->src_next += amount;
}

/**
 * Skips over the following character if it is <code>c</code>.
 * Won't cause an error if the end is reached.
 * Returns the amount of characters skipped.
 * Undefined behaviour if <code>c == HB_EOF</code>.
 *
 * @param proc proc
 * @param c character to skip if next
 * @return 1 if skipped, 0 otherwise
 */
size_t hb_proc_skip_if(hb_proc* proc, hb_rune c) {
    hb_eof_rune n = hb_proc_peek_eof(proc);

    // n != c takes care of n == HB_EOF
    if (n != c) {
        return 0;
    }

    proc->src_next++;

    return 1;
}

/**
 * Skips over every following character until one dissatisfies the predicate <code>pred</code>,
 * or the end is reached.
 *
 * @param proc proc
 * @param pred predicate
 * @return amount of characters skipped
 */
size_t hb_proc_skip_while_predicate(hb_proc* proc, hb_proc_pred* pred) {
    size_t count = 0;

    while (true) {
        hb_eof_rune c = hb_proc_peek_eof(proc);

        if (c == HB_EOF || !(*pred)(c)) {
            break;
        }

        count++;
    }

    proc->src_next += count;

    return count;
}

/**
 * Skips over the next sequence of characters if they are <code>match</code>.
 * Requires that the file has at least the length of <code>match</code> characters remaining.
 * Returns the amount of characters skipped.
 * Undefined behaviour if <code>match</code> is empty.
 *
 * @param proc proc
 * @param match sequence of characters to match
 * @return 0 if not matched, length of <code>match</code> otherwise
 * @throws on HBE_PARSE_UNEXPECTED_END
 */
size_t hb_proc_skip_if_matches(hb_proc* proc, char const* match) {
    size_t match_len = hb_proc_matches(proc, match);

    if (match_len) {
        proc->src_next += match_len;
    }

    return match_len;
}
