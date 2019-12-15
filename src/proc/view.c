#include <hb/collection.h>
#include <hb/proc.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

// A view represents a substring of the source. Faster, easier, safer, and more
// efficient than making a copy. If the end is before the start, it's invalid,
// like NaN. Can be used for special meaning. See lib/nicehash/view-str.h for
// more details.

// To avoid underflow, there are no hb_proc_view_start_with_*_prev functions.

// Start a view at the position of the next character to consume.
void hb_proc_view_start_with_src_next(nh_view_str* view, hb_proc* proc)
{
	nh_view_str_set_start(view, proc->src_next);
}

// End a view at the position of the last character consumed (inclusive).
void hb_proc_view_end_with_src_prev(nh_view_str* view, hb_proc* proc)
{
	nh_view_str_set_length(view, proc->src_next <= view->start
					     ? 0
					     : proc->src_next - view->start);
}

// Start a view at the position of the next character that will have been
// processed.
void hb_proc_view_start_with_out_next(nh_view_str* view, hb_proc* proc)
{
	nh_view_str_set_start(view, proc->out_next);
}

// End a view at the position of the last character processed (inclusive).
void hb_proc_view_end_with_out_prev(nh_view_str* view, hb_proc* proc)
{
	nh_view_str_set_length(view, proc->out_next <= view->start
					     ? 0
					     : proc->out_next - view->start);
}
