#pragma once

#include <hb/collection.h>
#include <hb/rune.h>
#include <hb/rule.h>

static nh_set_int32* hb_rule_ascii_whitespace_set;

void hb_rule_ascii_whitespace_add_elems(nh_set_int32* set) {
    nh_set_int32_add(set, 0x09); // TAB
    nh_set_int32_add(set, 0x0A); // LF
    nh_set_int32_add(set, 0x0C); // FF
    nh_set_int32_add(set, 0x0D); // CR
    nh_set_int32_add(set, 0x20); // SPACE
}

void hb_rule_ascii_whitespace_init(void) {
    hb_rule_ascii_whitespace_set = nh_set_int32_create();
    hb_rule_ascii_whitespace_add_elems(hb_rule_ascii_whitespace_set);
}

bool hb_rule_ascii_whitespace_check(hb_rune c) {
    return nh_set_int32_has(hb_rule_ascii_whitespace_set, c);
}
