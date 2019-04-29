#include <hb/err.h>
#include <hb/proc.h>
#include <hb/rune.h>

/**
 * Require the next character to be `c`.
 * The matched character is written to output.
 *
 * @param proc proc
 * @param c character to match
 * @throws on HB_ERR_PARSE_UNEXPECTED_END or HB_ERR_PARSE_EXPECTED_NOT_FOUND
 */
void hb_proc_require(hb_proc* proc, hb_rune c)
{
	hb_rune n = hb_proc_accept(proc);

	if (c != n) {
		hb_proc_error_custom(proc, HB_ERR_PARSE_EXPECTED_NOT_FOUND,
				     "Expected `%c` (U+%x), got `%c` (U+%x)", c,
				     c, n, n);
	}
}

/**
 * Require the next character to be `c`.
 * The matched character is skipped over and NOT written to output, and also
 * returned.
 *
 * @param proc proc
 * @param c character to match
 * @return matched character
 * @throws on HB_ERR_PARSE_UNEXPECTED_END or HB_ERR_PARSE_EXPECTED_NOT_FOUND
 */
hb_rune hb_proc_require_skip(hb_proc* proc, hb_rune c)
{
	hb_rune n = hb_proc_skip(proc);

	if (c != n) {
		hb_proc_error_custom(
			proc, HB_ERR_PARSE_EXPECTED_NOT_FOUND,
			"Expected `%c` (U+%x), got `%c` (U+%x) at %s", c, c, n,
			n);
	}

	return n;
}

/**
 * Require the next character to satisfy the predicate `pred`.
 * The matched character is written to output.
 * If not matched, the error message will describe the expected output using
 * `name`.
 *
 * @param proc proc
 * @param pred predicate
 * @param name what to output in the error message to describe the requirement
 * @return required character
 * @throws HB_ERR_PARSE_UNEXPECTED_END or HB_ERR_PARSE_EXPECTED_NOT_FOUND
 */
hb_rune hb_proc_require_predicate(hb_proc* proc, hb_proc_pred* pred,
				  char const* name)
{
	hb_rune n = hb_proc_accept(proc);

	if (!(*pred)(n)) {
		hb_proc_error_custom(proc, HB_ERR_PARSE_EXPECTED_NOT_FOUND,
				     "Expected %s, got `%c` (U+%x)", name, n,
				     n);
	}

	return n;
}

/**
 * Require the next character to satisfy the predicate `pred`.
 * The matched character is skipped over and NOT written to output.
 * If not matched, the error message will describe the expected output using
 * `name`.
 *
 * @param proc proc
 * @param pred predicate
 * @param name what to output in the error message to describe the requirement
 * @return required character
 * @throws on HB_ERR_PARSE_UNEXPECTED_END or HB_ERR_PARSE_EXPECTED_NOT_FOUND
 */
hb_rune hb_proc_require_skip_predicate(hb_proc* proc, hb_proc_pred* pred,
				       char const* name)
{
	hb_rune n = hb_proc_skip(proc);

	if (!(*pred)(n)) {
		hb_proc_error_custom(proc, HB_ERR_PARSE_EXPECTED_NOT_FOUND,
				     "Expected %s, got `%c` (U+%x)", name, n,
				     n);
	}

	return n;
}

/**
 * Require the next sequence of characters to be equal to `match`.
 * Matched characters are written to output.
 *
 * @param proc proc
 * @param match sequence of characters to require
 * @param match_len length of {@arg match}
 * @throws on HB_ERR_PARSE_UNEXPECTED_END or HB_ERR_PARSE_EXPECTED_NOT_FOUND
 */
void hb_proc_require_match_len(hb_proc* proc, char const* match,
			       size_t match_len)
{
	if (!hb_proc_accept_if_matches_len(proc, match, match_len)) {
		hb_proc_error_custom(proc, HB_ERR_PARSE_EXPECTED_NOT_FOUND,
				     "Expected `%s`", match);
	}
}

/**
 * Require the next sequence of characters to be equal to `match`.
 * Matched characters are skipped over and NOT written to output.
 *
 * @param proc proc
 * @param match sequence of characters to require
 * @param match_len length of {@arg match}
 * @throws on HB_ERR_PARSE_UNEXPECTED_END or HB_ERR_PARSE_EXPECTED_NOT_FOUND
 */
void hb_proc_require_skip_match_len(hb_proc* proc, char const* match,
				    size_t match_len)
{
	if (!hb_proc_matches_len(proc, match, match_len)) {
		hb_proc_error_custom(proc, HB_ERR_PARSE_EXPECTED_NOT_FOUND,
				     "Expected `%s`", match);
	}

	hb_proc_skip_amount(proc, match_len);
}
