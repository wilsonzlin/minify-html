#pragma once

#include <hb/collection.h>
#include <hb/rune.h>
#include <hb/rule.h>

static nh_set_int32* hb_rule_ascii_uppercase_set;

void hb_rule_ascii_uppercase_add_elems(nh_set_int32* set) {
    for (char c = 'A'; c <= 'Z'; c++) {
        nh_set_int32_add(set, c);
    }
}

void hb_rule_ascii_uppercase_init(void) {
    hb_rule_ascii_uppercase_set = nh_set_int32_create();
    hb_rule_ascii_uppercase_add_elems(hb_rule_ascii_uppercase_set);
}

bool hb_rule_ascii_uppercase_check(hb_rune c) {
    return nh_set_int32_has(hb_rule_ascii_uppercase_set, c);
}
