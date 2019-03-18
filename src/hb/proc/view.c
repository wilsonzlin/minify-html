#pragma once

#include <stdbool.h>
#include <hb/proc.h>

bool hb_proc_view_invalid(hb_proc_view view) {
    return view.end <= view.start;
}
