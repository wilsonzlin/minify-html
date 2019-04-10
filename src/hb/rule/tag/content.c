#pragma once

#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static nh_set_str* hb_rule_tag_content_set;

void hb_rule_tag_content_add_elems(nh_set_str* set) {
    nh_set_str_add(set, "address");
    nh_set_str_add(set, "audio");
    nh_set_str_add(set, "button");
    nh_set_str_add(set, "canvas");
    nh_set_str_add(set, "caption");
    nh_set_str_add(set, "figcaption");
    nh_set_str_add(set, "h1");
    nh_set_str_add(set, "h2");
    nh_set_str_add(set, "h3");
    nh_set_str_add(set, "h4");
    nh_set_str_add(set, "h5");
    nh_set_str_add(set, "h6");
    nh_set_str_add(set, "legend");
    nh_set_str_add(set, "meter");
    nh_set_str_add(set, "object");
    nh_set_str_add(set, "option");
    nh_set_str_add(set, "p");
    nh_set_str_add(set, "summary"); // Can also contain a heading
    nh_set_str_add(set, "textarea");
    nh_set_str_add(set, "video");
}

void hb_rule_tag_content_init(void) {
    hb_rule_tag_content_set = nh_set_str_create();
    hb_rule_tag_content_add_elems(hb_rule_tag_content_set);
}

bool hb_rule_tag_content_check(hb_rune* tag) {
    return nh_set_str_has(hb_rule_tag_content_set, tag);
}
