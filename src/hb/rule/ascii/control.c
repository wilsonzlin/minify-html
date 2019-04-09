#pragma once

#include <hb/collection.h>
#include <hb/rune.h>
#include <hb/rule.h>

static nh_set_int32* hb_rule_ascii_control_set;

void hb_rule_ascii_control_add_elems(nh_set_int32* set) {
    for (char c = 0x0; c <= 0x1F; c++) {
        nh_set_int32_add(set, c);
    }
}

void hb_rule_ascii_control_init(void) {
    hb_rule_ascii_control_set = nh_set_int32_create();
    hb_rule_ascii_control_add_elems(hb_rule_ascii_control_set);
}

bool hb_rule_ascii_control_check(hb_rune c) {
    return nh_set_int32_has(hb_rule_ascii_control_set, c);
}
