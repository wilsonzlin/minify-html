#pragma once

#include <hb/collection.h>
#include <hb/rune.h>
#include <hb/rule.h>

static nh_set_int32* hb_rule_attr_quote_set;

void hb_rule_attr_quote_add_elems(nh_set_int32* set) {
    nh_set_int32_add(set, '\'');
    nh_set_int32_add(set, '"');
}

void hb_rule_attr_quote_init(void) {
    hb_rule_attr_quote_set = nh_set_int32_create();
    hb_rule_attr_quote_add_elems(hb_rule_attr_quote_set);
}

bool hb_rule_attr_quote_check(hb_rune c) {
    return nh_set_int32_has(hb_rule_attr_quote_set, c);
}
