#pragma once

#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static nh_set_str* hb_rule_tag_sectioning_set;

void hb_rule_tag_sectioning_add_elems(nh_set_str* set) {
    nh_set_str_add(set, "article");
    nh_set_str_add(set, "aside");
    nh_set_str_add(set, "nav");
    nh_set_str_add(set, "section");
}

void hb_rule_tag_sectioning_init(void) {
    hb_rule_tag_sectioning_set = nh_set_str_create();
    hb_rule_tag_sectioning_add_elems(hb_rule_tag_sectioning_set);
}

bool hb_rule_tag_sectioning_check(hb_rune* tag) {
    return nh_set_str_has(hb_rule_tag_sectioning_set, tag);
}
