#pragma once

#include <hb/collection.h>
#include <hb/rune.h>
#include <hb/rule.h>

static nh_set_int32* hb_rule_ascii_lowercase_set;

void hb_rule_ascii_lowercase_add_elems(nh_set_int32* set) {
    for (char c = 'a'; c <= 'z'; c++) {
        nh_set_int32_add(set, c);
    }
}

void hb_rule_ascii_lowercase_init(void) {
    hb_rule_ascii_lowercase_set = nh_set_int32_create();
    hb_rule_ascii_lowercase_add_elems(hb_rule_ascii_lowercase_set);
}

bool hb_rule_ascii_lowercase_check(hb_rune c) {
    return nh_set_int32_has(hb_rule_ascii_lowercase_set, c);
}
