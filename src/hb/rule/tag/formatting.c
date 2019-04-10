#pragma once

#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static nh_set_str* hb_rule_tag_formatting_set;

void hb_rule_tag_formatting_add_elems(nh_set_str* set) {
    // Difference to MDN's inline text semantics list: -br, +del, +ins
    nh_set_str_add(set, "a");
    nh_set_str_add(set, "abbr");
    nh_set_str_add(set, "b");
    nh_set_str_add(set, "bdi");
    nh_set_str_add(set, "bdo");
    nh_set_str_add(set, "cite");
    nh_set_str_add(set, "data");
    nh_set_str_add(set, "del");
    nh_set_str_add(set, "dfn");
    nh_set_str_add(set, "em");
    nh_set_str_add(set, "i");
    nh_set_str_add(set, "ins");
    nh_set_str_add(set, "kbd");
    nh_set_str_add(set, "mark");
    nh_set_str_add(set, "q");
    nh_set_str_add(set, "rp");
    nh_set_str_add(set, "rt");
    nh_set_str_add(set, "rtc");
    nh_set_str_add(set, "ruby");
    nh_set_str_add(set, "s");
    nh_set_str_add(set, "samp");
    nh_set_str_add(set, "small");
    nh_set_str_add(set, "span");
    nh_set_str_add(set, "strong");
    nh_set_str_add(set, "sub");
    nh_set_str_add(set, "sup");
    nh_set_str_add(set, "time");
    nh_set_str_add(set, "u");
    nh_set_str_add(set, "var");
    nh_set_str_add(set, "wbr");
}

void hb_rule_tag_formatting_init(void) {
    hb_rule_tag_formatting_set = nh_set_str_create();
    hb_rule_tag_formatting_add_elems(hb_rule_tag_formatting_set);
}

bool hb_rule_tag_formatting_check(hb_rune* tag) {
    return nh_set_str_has(hb_rule_tag_formatting_set, tag);
}
