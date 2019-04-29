#include <hb/proc.h>
#include <hb/rune.h>
#include <stdbool.h>
#include <string.h>

/**
 * Accept the next character.
 * Will cause an error if already at end.
 *
 * @param proc proc
 * @return next character
 * @throws on HB_ERR_PARSE_UNEXPECTED_END
 */
hb_rune hb_proc_accept(hb_proc* proc)
{
	// Get the next character, throwing if EOF.
	hb_rune c = hb_proc_peek(proc);

	// Append to output.
	hb_proc_write(proc, c);

	// Mark character as consumed.
	proc->src_next++;

	return c;
}

/**
 * Accept the next `count` characters.
 * Requires at least `count` characters remaining.
 *
 * @param proc proc
 * @param count amount of characters
 * @throws on HB_ERR_PARSE_UNEXPECTED_END
 */
void hb_proc_accept_count(hb_proc* proc, size_t count)
{
	hb_proc_bounds_assert_offset(proc, count);

	memcpy(&proc->out[proc->out_next], &proc->src[proc->src_next], count);

	proc->src_next += count;
	proc->out_next += count;
}

/**
 * Accept the following character if it is `c`.
 * Won't match or cause an error if there are no characters remaining.
 * Undefined behaviour if `c == HB_EOF`.
 *
 * @param proc proc
 * @param c character to match
 * @return false if nothing was accepted, true otherwise
 */
bool hb_proc_accept_if(hb_proc* proc, hb_rune c)
{
	hb_eof_rune n = hb_proc_peek_eof(proc);

	// n != c takes care of n == HB_EOF
	if (n != c) {
		return false;
	}

	hb_proc_accept(proc);

	return true;
}

/**
 * Accept the following character if it is not `c`.
 * Won't match or cause an error if there are no characters remaining.
 * Undefined behaviour if `c == HB_EOF`.
 *
 * @param proc proc
 * @param c character to not match
 * @return false if nothing was accepted, true otherwise
 */
bool hb_proc_accept_if_not(hb_proc* proc, hb_rune c)
{
	hb_eof_rune n = hb_proc_peek_eof(proc);

	// n == c takes care of n != HB_EOF
	if (n == c) {
		return false;
	}

	hb_proc_accept(proc);

	return true;
}

/**
 * Accept the following characters if they match `match`.
 * Won't match or cause an error if there are not enough characters remaining.
 * If `match` has a length of zero, behaviour is undefined.
 *
 * @param proc proc
 * @param match characters to match
 * @param match_len length of {@arg match}
 * @return 0 if nothing was accepted, length of `match` otherwise
 */
size_t hb_proc_accept_if_matches_len(hb_proc* proc, char const* match,
				     size_t match_len)
{
	if (hb_proc_matches_len(proc, match, match_len)) {
		hb_proc_accept_count(proc, match_len);
	}

	return match_len;
}

/**
 * Accept the following characters if they are either "\r", "\r\n", or "\n".
 * Won't cause an error if insufficient amount of characters left.
 *
 * @param proc proc
 * @return amount of characters matched
 */
size_t hb_proc_accept_if_matches_line_terminator(hb_proc* proc)
{
	size_t match_len = hb_proc_matches_line_terminator(proc);

	if (match_len) {
		hb_proc_accept_count(proc, match_len);
	}

	return match_len;
}

/**
 * Accept the following character if it satisfies the predicate `pred`.
 * Won't do anything if already at the end.
 *
 * @param proc proc
 * @param pred predicate
 * @return false if nothing was accepted, true otherwise
 */
bool hb_proc_accept_if_predicate(hb_proc* proc, hb_proc_pred* pred)
{
	hb_eof_rune c = hb_proc_peek_eof(proc);

	if (c == HB_EOF || !(*pred)((hb_rune) c)) {
		return false;
	}

	hb_proc_accept(proc);

	return true;
}

/**
 * Accept every following character until one dissatisfies the predicate `pred`,
 * or the end is reached.
 *
 * @param proc proc
 * @param pred predicate
 * @return amount of characters accepted
 */
size_t hb_proc_accept_while_predicate(hb_proc* proc, hb_proc_pred* pred)
{
	size_t count = 0;

	while (hb_proc_accept_if_predicate(proc, pred)) {
		count++;
	}

	return count;
}
