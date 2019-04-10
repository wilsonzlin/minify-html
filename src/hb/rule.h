#pragma once

#include <hb/rune.h>
#include <hb/collection.h>

void hb_rule_init(void);

void hb_rule_ascii_control_add_elems(nh_set_int32* set);
void hb_rule_ascii_control_init(void);
bool hb_rule_ascii_control_check(hb_rune c);

void hb_rule_ascii_digit_add_elems(nh_set_int32* set);
void hb_rule_ascii_digit_init(void);
bool hb_rule_ascii_digit_check(hb_rune c);

void hb_rule_ascii_hex_add_elems(nh_set_int32* set);
void hb_rule_ascii_hex_init(void);
bool hb_rule_ascii_hex_check(hb_rune c);

void hb_rule_ascii_lowercase_add_elems(nh_set_int32* set);
void hb_rule_ascii_lowercase_init(void);
bool hb_rule_ascii_lowercase_check(hb_rune c);

void hb_rule_ascii_uppercase_add_elems(nh_set_int32* set);
void hb_rule_ascii_uppercase_init(void);
bool hb_rule_ascii_uppercase_check(hb_rune c);

void hb_rule_ascii_whitespace_add_elems(nh_set_int32* set);
void hb_rule_ascii_whitespace_init(void);
bool hb_rule_ascii_whitespace_check(hb_rune c);

void hb_rule_attr_name_add_exceptions(nh_set_int32* set);
void hb_rule_attr_name_init(void);
bool hb_rule_attr_name_check(hb_rune c);

void hb_rule_attr_quote_add_elems(nh_set_int32* set);
void hb_rule_attr_quote_init(void);
bool hb_rule_attr_quote_check(hb_rune c);

void hb_rule_attr_unquotedvalue_add_exceptions(nh_set_int32* set);
void hb_rule_attr_unquotedvalue_init(void);
bool hb_rule_attr_unquotedvalue_check(hb_rune c);

void hb_rule_entity_reference_map_add_entries(nh_map_str_int32* map);
void hb_rule_entity_reference_init(void);
bool hb_rule_entity_reference_exists(hb_rune* ref);
int32_t hb_rule_entity_reference_map_get(hb_rune* ref);

void hb_rule_tag_content_add_elems(nh_set_str* set);
void hb_rule_tag_content_init(void);
bool hb_rule_tag_content_check(hb_rune* tag);

void hb_rule_tag_contentfirst_add_elems(nh_set_str* set);
void hb_rule_tag_contentfirst_init(void);
bool hb_rule_tag_contentfirst_check(hb_rune* tag);

void hb_rule_tag_formatting_add_elems(nh_set_str* set);
void hb_rule_tag_formatting_init(void);
bool hb_rule_tag_formatting_check(hb_rune* tag);

void hb_rule_tag_heading_add_elems(nh_set_str* set);
void hb_rule_tag_heading_init(void);
bool hb_rule_tag_heading_check(hb_rune* tag);

void hb_rule_tag_html_add_elems(nh_set_str* set);
void hb_rule_tag_html_init(void);
bool hb_rule_tag_html_check(hb_rune* tag);

void hb_rule_tag_layout_add_elems(nh_set_str* set);
void hb_rule_tag_layout_init(void);
bool hb_rule_tag_layout_check(hb_rune* tag);

void hb_rule_tag_media_add_elems(nh_set_str* set);
void hb_rule_tag_media_init(void);
bool hb_rule_tag_media_check(hb_rune* tag);

void hb_rule_tag_name_add_elems(nh_set_int32* set);
void hb_rule_tag_name_init(void);
bool hb_rule_tag_name_check(hb_rune c);

void hb_rule_tag_sectioning_add_elems(nh_set_str* set);
void hb_rule_tag_sectioning_init(void);
bool hb_rule_tag_sectioning_check(hb_rune* tag);

void hb_rule_tag_specific_add_elems(nh_set_str* set);
void hb_rule_tag_specific_init(void);
bool hb_rule_tag_specific_check(hb_rune* tag);

void hb_rule_tag_svg_add_elems(nh_set_str* set);
void hb_rule_tag_svg_init(void);
bool hb_rule_tag_svg_check(hb_rune* tag);

bool hb_rule_tag_valid_check(hb_rune* tag);

void hb_rule_tag_void_add_elems(nh_set_str* set);
void hb_rule_tag_void_init(void);
bool hb_rule_tag_void_check(hb_rune* tag);

void hb_rule_tag_wss_add_elems(nh_set_str* set);
void hb_rule_tag_wss_init(void);
bool hb_rule_tag_wss_check(hb_rune* tag);

void hb_rule_tag_child_blacklist_map_add_entries(nh_map_str_set_str* map);
void hb_rule_tag_child_blacklist_init(void);
bool hb_rule_tag_child_blacklist_allowed(hb_rune* parent, hb_rune* child);

void hb_rule_tag_child_whitelist_map_add_entries(nh_map_str_set_str* map);
void hb_rule_tag_child_whitelist_init(void);
bool hb_rule_tag_child_whitelist_allowed(hb_rune* parent, hb_rune* child);

void hb_rule_tag_parent_blacklist_init(void);
bool hb_rule_tag_parent_blacklist_allowed(hb_rune* child, hb_rune* parent);

void hb_rule_tag_parent_whitelist_map_add_entries(nh_map_str_set_str* map);
void hb_rule_tag_parent_whitelist_init(void);
bool hb_rule_tag_parent_whitelist_allowed(hb_rune* child, hb_rune* parent);
