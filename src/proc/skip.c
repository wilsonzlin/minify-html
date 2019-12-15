#include <hb/proc.h>
#include <hb/rune.h>

/**
 * Skip over the next character.
 * Requires that the file has at least one character remaining.
 *
 * @param proc proc
 * @return skipped character
 * @throws on HB_ERR_PARSE_UNEXPECTED_END
 */
hb_rune hb_proc_skip(hb_proc* proc)
{
	hb_proc_bounds_assert_not_eof(proc);

	hb_rune c = proc->src[proc->src_next];

	proc->src_next++;

	return c;
}

/**
 * Skip over the next `amount` characters.
 * Requires that the file has at least `amount` characters remaining.
 *
 * @param proc proc
 * @param amount amount of characters to skip
 * @return amount of characters skipped
 * @throws on HB_ERR_PARSE_UNEXPECTED_END
 */
size_t hb_proc_skip_amount(hb_proc* proc, size_t amount)
{
	hb_proc_bounds_assert_offset(proc, amount);

	proc->src_next += amount;

	return amount;
}

/**
 * Skip over the following character if it is `c`.
 * Won't cause an error if the end is reached.
 * Returns the amount of characters skipped.
 * Undefined behaviour if `c == HB_EOF`.
 *
 * @param proc proc
 * @param c character to skip if next
 * @return 1 if skipped, 0 otherwise
 */
size_t hb_proc_skip_if(hb_proc* proc, hb_rune c)
{
	hb_eof_rune n = hb_proc_peek_eof(proc);

	// n != c takes care of n == HB_EOF
	if (n != c) {
		return 0;
	}

	proc->src_next++;

	return 1;
}

/**
 * Skip over every following character until one dissatisfies the predicate
 * `pred`, or the end is reached.
 *
 * @param proc proc
 * @param pred predicate
 * @return amount of characters skipped
 */
size_t hb_proc_skip_while_predicate(hb_proc* proc, hb_proc_pred* pred)
{
	size_t count = 0;

	while (true) {
		hb_eof_rune c = hb_proc_peek_eof_offset(proc, count);

		if (c == HB_EOF || !(*pred)(c)) {
			break;
		}

		count++;
	}

	proc->src_next += count;

	return count;
}
