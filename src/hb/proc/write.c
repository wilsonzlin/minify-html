#pragma once

#include <hb/proc.h>

void hb_proc_write(hb_proc* proc, hb_rune c) {
    // WARNING: Does not check if out_next exceeds bounds.
    proc->out[proc->out_next] = c;
    proc->out_next++;
}
