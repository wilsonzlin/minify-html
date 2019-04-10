#pragma once

#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

// "wss" stands for whitespace-sensitive.

static nh_set_str* hb_rule_tag_wss_set;

void hb_rule_tag_wss_add_elems(nh_set_str* set) {
    nh_set_str_add(set, "code");
    nh_set_str_add(set, "pre");
}

void hb_rule_tag_wss_init(void) {
    hb_rule_tag_wss_set = nh_set_str_create();
    hb_rule_tag_wss_add_elems(hb_rule_tag_wss_set);
}

bool hb_rule_tag_wss_check(hb_rune* tag) {
    return nh_set_str_has(hb_rule_tag_wss_set, tag);
}
