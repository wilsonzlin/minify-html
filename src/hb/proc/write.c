#include <hb/proc.h>

void hb_proc_write(hb_proc* proc, hb_rune c)
{
	// WARNING: Does not check if out_next exceeds bounds.
	proc->out[proc->out_next] = c;
	proc->out_next++;
}

void hb_proc_write_view(hb_proc* proc, nh_view_str* view)
{
	// WARNING: Does not check boundaries.
	// WARNING: This works because nh_view_str and proc->out have the same
	// element types. Be aware should this change.
	memcpy(&proc->out[proc->out_next], &view->array[view->start],
	       view->length * sizeof(hb_rune));
	proc->out_next += view->length;
}

size_t hb_proc_write_utf_8(hb_proc* proc, uint32_t c)
{
	if (c <= 0x7F) {
		// Plain ASCII.
		hb_proc_write(proc, (hb_rune) c);
		return 1;
	}

	if (c <= 0x07FF) {
		// 2-byte UTF-8.
		hb_proc_write(proc, (hb_rune)(((c >> 6) & 0x1F) | 0xC0));
		hb_proc_write(proc, (hb_rune)(((c >> 0) & 0x3F) | 0x80));
		return 2;
	}

	if (c <= 0xFFFF) {
		// 3-byte UTF-8.
		hb_proc_write(proc, (hb_rune)(((c >> 12) & 0x0F) | 0xE0));
		hb_proc_write(proc, (hb_rune)(((c >> 6) & 0x3F) | 0x80));
		hb_proc_write(proc, (hb_rune)(((c >> 0) & 0x3F) | 0x80));
		return 3;
	}

	if (c <= 0x10FFFF) {
		// 4-byte UTF-8.
		hb_proc_write(proc, (hb_rune)(((c >> 18) & 0x07) | 0xF0));
		hb_proc_write(proc, (hb_rune)(((c >> 12) & 0x3F) | 0x80));
		hb_proc_write(proc, (hb_rune)(((c >> 6) & 0x3F) | 0x80));
		hb_proc_write(proc, (hb_rune)(((c >> 0) & 0x3F) | 0x80));
		return 4;
	}

	return 0;
}
