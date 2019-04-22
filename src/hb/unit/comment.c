#include <hb/unit.h>

void hb_unit_comment(hb_proc* proc) {
    // Mark comment to write it later if not removing comments.
    hb_proc_view_init_src(comment, proc);

    hb_proc_view_start_with_src_next(&comment, proc);
    hb_proc_require_skip_match(proc, "<!--");
    while (!hb_proc_skip_if_matches(proc, "-->")) {
        hb_proc_skip(proc);
    }
    hb_proc_view_end_with_src_prev(&comment, proc);

    // Write comment if not removing comments.
    if (proc->cfg->remove_comments) {
        hb_proc_write_view(proc, &comment);
    }
}
