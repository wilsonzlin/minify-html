#pragma once

#include <hb/rule.h>

void hb_rule_init(void) {
    // Core
    hb_rule_ascii_control_init();
    hb_rule_ascii_digit_init();
    hb_rule_ascii_hex_init();
    hb_rule_ascii_uppercase_init();
    hb_rule_ascii_lowercase_init();
    hb_rule_ascii_whitespace_init();

    // Identifiers
    hb_rule_tag_name_init();
    hb_rule_attr_name_init();

    // Values
    hb_rule_attr_quote_init();
    hb_rule_attr_unquotedvalue_init();
    hb_rule_entity_reference_init();

    // Specification tag categories
    hb_rule_tag_heading_init();
    hb_rule_tag_media_init();
    hb_rule_tag_sectioning_init();

    hb_rule_tag_void_init();
    hb_rule_tag_wss_init();

    hb_rule_tag_html_init();
    hb_rule_tag_svg_init();

    // Hyperbuild tag categories
    hb_rule_tag_contentfirst_init();
    hb_rule_tag_content_init();
    hb_rule_tag_formatting_init();
    hb_rule_tag_layout_init();
    hb_rule_tag_specific_init();

    // Relations
    hb_rule_tag_child_blacklist_init();
    hb_rule_tag_child_whitelist_init();
    hb_rule_tag_parent_blacklist_init();
    hb_rule_tag_parent_whitelist_init();
}
