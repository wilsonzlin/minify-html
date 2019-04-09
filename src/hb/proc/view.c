#pragma once

#include <stdbool.h>
#include <hb/proc.h>

bool hb_proc_view_invalid(hb_proc_view view) {
    return view.end <= view.start;
}

void hb_proc_view_mark_start_with_prev(hb_proc_view* view, hb_proc* proc) {
    view->start = proc->src_next - 1;
}

void hb_proc_view_mark_start_with_next(hb_proc_view* view, hb_proc* proc) {
    view->start = proc->src_next;
}

void hb_proc_view_mark_end_with_prev(hb_proc_view* view, hb_proc* proc) {
    view->end = proc->src_next - 1;
}

void hb_proc_view_mark_end_with_next(hb_proc_view* view, hb_proc* proc) {
    view->end = proc->src_next;
}
