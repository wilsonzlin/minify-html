#pragma once

#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static nh_set_str* hb_rule_tag_specific_set;

void hb_rule_tag_specific_add_elems(nh_set_str* set) {
    hb_rule_tag_svg_add_elems(set);
    nh_set_str_add(set, "area");
    nh_set_str_add(set, "base");
    nh_set_str_add(set, "br");
    nh_set_str_add(set, "code"); // Reason: unlikely to want to minify
    nh_set_str_add(set, "col");
    nh_set_str_add(set, "embed");
    nh_set_str_add(set, "hr");
    nh_set_str_add(set, "img");
    nh_set_str_add(set, "input");
    nh_set_str_add(set, "param");
    nh_set_str_add(set, "pre"); // Reason: unlikely to want to minify
    nh_set_str_add(set, "script");
    nh_set_str_add(set, "source");
    nh_set_str_add(set, "track");
}

void hb_rule_tag_specific_init(void) {
    hb_rule_tag_specific_set = nh_set_str_create();
    hb_rule_tag_specific_add_elems(hb_rule_tag_specific_set);
}

bool hb_rule_tag_specific_check(hb_rune* tag) {
    return nh_set_str_has(hb_rule_tag_specific_set, tag);
}
