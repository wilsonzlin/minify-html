#pragma once

#include <hb/collection.h>
#include <hb/rune.h>
#include <hb/rule.h>

static nh_set_int32* hb_rule_attr_name_exceptions;

void hb_rule_attr_name_add_exceptions(nh_set_int32* set) {
    hb_rule_ascii_control_add_elems(set);
    nh_set_int32_add(set, ' ');
    nh_set_int32_add(set, '"');
    nh_set_int32_add(set, '\'');
    nh_set_int32_add(set, '>');
    nh_set_int32_add(set, '/');
    nh_set_int32_add(set, '=');
    // NOTE: Unicode noncharacters not tested (https://html.spec.whatwg.org/multipage/syntax.html#syntax-attribute-name)
}

void hb_rule_attr_name_init(void) {
    hb_rule_attr_name_exceptions = nh_set_int32_create();
    hb_rule_attr_name_add_exceptions(hb_rule_attr_name_exceptions);
}

bool hb_rule_attr_name_check(hb_rune c) {
    return !nh_set_int32_has(hb_rule_attr_name_exceptions, c);
}
