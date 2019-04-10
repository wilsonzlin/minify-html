#pragma once

#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static nh_map_str_set_str* hb_rule_tag_parent_blacklist_map;

void hb_rule_tag_parent_blacklist_init(void) {
    // Don't do anything. This rule is currently unused.
}

// Check if a child is allowed to have a specific parent, based on the blacklist.
bool hb_rule_tag_parent_blacklist_allowed(hb_rune* child, hb_rune* parent) {
    // Since this rule is currently not being used, directly allow without any checks.
    return true;
}
