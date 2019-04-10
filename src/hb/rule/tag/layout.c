#pragma once

#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static nh_set_str* hb_rule_tag_layout_set;

void hb_rule_tag_layout_add_elems(nh_set_str* set) {
    hb_rule_tag_sectioning_add_elems(set);
    nh_set_str_add(set, "blockquote");
    nh_set_str_add(set, "body");
    nh_set_str_add(set, "colgroup");
    nh_set_str_add(set, "datalist");
    nh_set_str_add(set, "dialog");
    nh_set_str_add(set, "div");
    nh_set_str_add(set, "dl");
    nh_set_str_add(set, "fieldset");
    nh_set_str_add(set, "figure");
    nh_set_str_add(set, "footer");
    nh_set_str_add(set, "form");
    nh_set_str_add(set, "head");
    nh_set_str_add(set, "header");
    nh_set_str_add(set, "hgroup");
    nh_set_str_add(set, "html");
    nh_set_str_add(set, "main");
    nh_set_str_add(set, "map");
    nh_set_str_add(set, "menu");
    nh_set_str_add(set, "nav");
    nh_set_str_add(set, "ol");
    nh_set_str_add(set, "optgroup");
    nh_set_str_add(set, "picture");
    nh_set_str_add(set, "section");
    nh_set_str_add(set, "select");
    nh_set_str_add(set, "table");
    nh_set_str_add(set, "tbody");
    nh_set_str_add(set, "tfoot");
    nh_set_str_add(set, "thead");
    nh_set_str_add(set, "tr");
    nh_set_str_add(set, "ul");
}

void hb_rule_tag_layout_init(void) {
    hb_rule_tag_layout_set = nh_set_str_create();
    hb_rule_tag_layout_add_elems(hb_rule_tag_layout_set);
}

bool hb_rule_tag_layout_check(hb_rune* tag) {
    return nh_set_str_has(hb_rule_tag_layout_set, tag);
}
