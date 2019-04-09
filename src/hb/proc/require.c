#pragma once

#include <hb/err.h>
#include <hb/rune.h>
#include <hb/proc.h>

/**
 * Requires the next character to be <code>c</code>.
 * The matched character is written to output.
 *
 * @param proc proc
 * @param c character to match
 * @throws on HBE_PARSE_UNEXPECTED_END, or HBE_PARSE_EXPECTED_NOT_FOUND
 */
void hb_proc_require(hb_proc* proc, hb_rune c) {
    hb_rune n = hb_proc_accept(proc);

    if (c != n) {
        hb_proc_error_custom(proc, HB_ERR_PARSE_EXPECTED_NOT_FOUND, "Expected `%c` (U+%x), got `%c` (U+%x)", c, c, n, n);
    }
}

/**
 * Requires the next character to be <code>c</code>.
 * The matched character is skipped over and NOT written to output, and also returned.
 *
 * @param proc proc
 * @param c character to match
 * @return matched character
 * @throws on HBE_PARSE_UNEXPECTED_END, or HBE_PARSE_EXPECTED_NOT_FOUND
 */
hb_rune hb_proc_require_skip(hb_proc* proc, hb_rune c) {
    hb_rune n = hb_proc_skip(proc);

    if (c != n) {
        hb_proc_error_custom(proc, HB_ERR_PARSE_EXPECTED_NOT_FOUND, "Expected `%c` (U+%x), got `%c` (U+%x) at %s", c, c, n, n);
    }

    return n;
}

/**
 * Requires the next character to satisfy the predicate <code>pred</code>.
 * The matched character is written to output.
 * If not matched, the error message will describe the expected output using <code>name</code>.
 *
 * @param proc proc
 * @param pred predicate
 * @param name what to output in the error message to describe the requirement
 * @return required character
 * @throws HBE_PARSE_UNEXPECTED_END, or HBE_PARSE_EXPECTED_NOT_FOUND
 */
hb_rune hb_proc_require_predicate(hb_proc* proc, hb_proc_pred* pred, char const* name) {
    hb_rune n = hb_proc_accept(proc);

    if (!(*pred)(n)) {
        hb_proc_error_custom(proc, HB_ERR_PARSE_EXPECTED_NOT_FOUND, "Expected %s, got `%c` (U+%x)", name, n, n);
    }

    return n;
}

/**
 * Requires the next character to satisfy the predicate <code>pred</code>.
 * The matched character is skipped over and NOT written to output.
 * If not matched, the error message will describe the expected output using <code>name</code>.
 *
 * @param proc proc
 * @param pred predicate
 * @param name what to output in the error message to describe the requirement
 * @return required character
 * @throws on HBE_PARSE_UNEXPECTED_END, or HBE_PARSE_EXPECTED_NOT_FOUND
 */
hb_rune hb_proc_require_skip_predicate(hb_proc* proc, hb_proc_pred* pred, char const* name) {
    hb_rune n = hb_proc_skip(proc);

    if (!(*pred)(n)) {
        hb_proc_error_custom(proc, HB_ERR_PARSE_EXPECTED_NOT_FOUND, "Expected %s, got `%c` (U+%x)", name, n, n);
    }

    return n;
}

/**
 * Requires the next sequence of characters to be equal to <code>match</code>.
 * Matched characters are written to output.
 *
 * @param proc proc
 * @param match sequence of characters to require
 * @throws on HBE_PARSE_UNEXPECTED_END, or HBE_PARSE_EXPECTED_NOT_FOUND
 */
void hb_proc_require_match(hb_proc* proc, char const* match) {
    size_t matches = hb_proc_accept_if_matches(proc, match);

    if (!matches) {
        hb_proc_error_custom(proc, HB_ERR_PARSE_EXPECTED_NOT_FOUND, "Expected `%s`", match);
    }
}

/**
 * Requires the next sequence of characters to be equal to <code>match</code>.
 * Matched characters are skipped over and NOT written to output.
 *
 * @param proc proc
 * @param match sequence of characters to require
 * @throws on HBE_PARSE_UNEXPECTED_END, or HBE_PARSE_EXPECTED_NOT_FOUND
 */
void hb_proc_require_skip_match(hb_proc* proc, char const* match) {
    size_t matches = hb_proc_skip_if_matches(proc, match);

    if (!matches) {
        hb_proc_error_custom(proc, HB_ERR_PARSE_EXPECTED_NOT_FOUND, "Expected `%s`", match);
    }
}
