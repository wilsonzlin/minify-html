#include <hb/unit.h>

void hb_unit_bang(hb_proc* proc) {
    hb_proc_require_match(proc, "<!");

    while (hb_proc_accept_if_not(proc, '<'));

    hb_proc_require(proc, '>');
}
