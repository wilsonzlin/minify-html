#include <hb/proc.h>
#include <hb/rune.h>
#include <stdbool.h>

/**
 * Assert that there are still unconsumed source characters remaining.
 *
 * @param proc proc
 * @throws HB_ERR_PARSE_UNEXPECTED_END if the end of the source has been reached
 */
void hb_proc_bounds_assert_not_eof(hb_proc* proc)
{
	if (proc->src_next == proc->src_len) {
		hb_proc_error(proc, HB_ERR_PARSE_UNEXPECTED_END,
			      "Unexpected end of input");
	}
}

/**
 * Check that `offset` characters from next does not exceed the end of the
 * source. When `offset` is 0, it represents the next unconsumed character.
 *
 * @param proc proc
 * @param offset
 * @return true if src_next + offset <= src_len
 */
bool hb_proc_bounds_check_offset(hb_proc* proc, size_t offset)
{
	return proc->src_next + offset <= proc->src_len;
}

/**
 * Assert that `offset` characters from next does not exceed the end of the
 * source. When `offset` is 0, it represents the next unconsumed character.
 *
 * @param proc proc
 * @param offset
 * @throws HB_ERR_PARSE_UNEXPECTED_END if `offset` exceeds end
 */
void hb_proc_bounds_assert_offset(hb_proc* proc, size_t offset)
{
	if (!hb_proc_bounds_check_offset(proc, offset)) {
		hb_proc_error(proc, HB_ERR_PARSE_UNEXPECTED_END,
			      "Unexpected end of input");
	}
}
