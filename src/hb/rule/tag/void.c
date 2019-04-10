#pragma once

#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static nh_set_str* hb_rule_tag_void_set;

void hb_rule_tag_void_add_elems(nh_set_str* set) {
    nh_set_str_add(set, "area");
    nh_set_str_add(set, "base");
    nh_set_str_add(set, "br");
    nh_set_str_add(set, "col");
    nh_set_str_add(set, "embed");
    nh_set_str_add(set, "hr");
    nh_set_str_add(set, "img");
    nh_set_str_add(set, "input");
    nh_set_str_add(set, "keygen");
    nh_set_str_add(set, "link");
    nh_set_str_add(set, "meta");
    nh_set_str_add(set, "param");
    nh_set_str_add(set, "source");
    nh_set_str_add(set, "track");
    nh_set_str_add(set, "wbr");
}

void hb_rule_tag_void_init(void) {
    hb_rule_tag_void_set = nh_set_str_create();
    hb_rule_tag_void_add_elems(hb_rule_tag_void_set);
}

bool hb_rule_tag_void_check(hb_rune* tag) {
    return nh_set_str_has(hb_rule_tag_void_set, tag);
}
