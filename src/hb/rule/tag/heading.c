#pragma once

#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static nh_set_str* hb_rule_tag_heading_set;

void hb_rule_tag_heading_add_elems(nh_set_str* set) {
    nh_set_str_add(set, "hgroup");
    nh_set_str_add(set, "h1");
    nh_set_str_add(set, "h2");
    nh_set_str_add(set, "h3");
    nh_set_str_add(set, "h4");
    nh_set_str_add(set, "h5");
    nh_set_str_add(set, "h6");
}

void hb_rule_tag_heading_init(void) {
    hb_rule_tag_heading_set = nh_set_str_create();
    hb_rule_tag_heading_add_elems(hb_rule_tag_heading_set);
}

bool hb_rule_tag_heading_check(hb_rune* tag) {
    return nh_set_str_has(hb_rule_tag_heading_set, tag);
}
