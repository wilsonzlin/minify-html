#pragma once

#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static nh_set_str* hb_rule_tag_media_set;

void hb_rule_tag_media_add_elems(nh_set_str* set) {
    nh_set_str_add(set, "audio");
    nh_set_str_add(set, "video");
}

void hb_rule_tag_media_init(void) {
    hb_rule_tag_media_set = nh_set_str_create();
    hb_rule_tag_media_add_elems(hb_rule_tag_media_set);
}

bool hb_rule_tag_media_check(hb_rune* tag) {
    return nh_set_str_has(hb_rule_tag_media_set, tag);
}
