#pragma once

#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static nh_set_str* hb_rule_tag_contentfirst_set;

void hb_rule_tag_contentfirst_add_elems(nh_set_str* set) {
    nh_set_str_add(set, "dd");
    nh_set_str_add(set, "details");
    nh_set_str_add(set, "dt");
    nh_set_str_add(set, "iframe");
    nh_set_str_add(set, "label");
    nh_set_str_add(set, "li");
    nh_set_str_add(set, "noscript");
    nh_set_str_add(set, "output");
    nh_set_str_add(set, "progress");
    nh_set_str_add(set, "slot");
    nh_set_str_add(set, "td");
    nh_set_str_add(set, "template");
    nh_set_str_add(set, "th");
}

void hb_rule_tag_contentfirst_init(void) {
    hb_rule_tag_contentfirst_set = nh_set_str_create();
    hb_rule_tag_contentfirst_add_elems(hb_rule_tag_contentfirst_set);
}

bool hb_rule_tag_contentfirst_check(hb_rune* tag) {
    return nh_set_str_has(hb_rule_tag_contentfirst_set, tag);
}
