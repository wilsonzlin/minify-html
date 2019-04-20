#pragma once

#include <hb/rune.h>
#include <hb/collection.h>

void hb_rule_init(void);

void hb_rule_ascii_control_add_elems(nh_bitfield_ascii* set);
void hb_rule_ascii_control_init(void);
bool hb_rule_ascii_control_check(hb_rune c);

void hb_rule_ascii_digit_add_elems(nh_bitfield_ascii* set);
void hb_rule_ascii_digit_init(void);
bool hb_rule_ascii_digit_check(hb_rune c);

void hb_rule_ascii_hex_add_elems(nh_bitfield_ascii* set);
void hb_rule_ascii_hex_init(void);
bool hb_rule_ascii_hex_check(hb_rune c);

void hb_rule_ascii_lowercase_add_elems(nh_bitfield_ascii* set);
void hb_rule_ascii_lowercase_init(void);
bool hb_rule_ascii_lowercase_check(hb_rune c);

void hb_rule_ascii_uppercase_add_elems(nh_bitfield_ascii* set);
void hb_rule_ascii_uppercase_init(void);
bool hb_rule_ascii_uppercase_check(hb_rune c);

void hb_rule_ascii_whitespace_add_elems(nh_bitfield_ascii* set);
void hb_rule_ascii_whitespace_init(void);
bool hb_rule_ascii_whitespace_check(hb_rune c);

void hb_rule_attr_name_add_exceptions(nh_bitfield_ascii* set);
void hb_rule_attr_name_init(void);
bool hb_rule_attr_name_check(hb_rune c);

void hb_rule_attr_quote_add_elems(nh_bitfield_ascii* set);
void hb_rule_attr_quote_init(void);
bool hb_rule_attr_quote_check(hb_rune c);

void hb_rule_attr_unquotedvalue_add_exceptions(nh_bitfield_ascii* set);
void hb_rule_attr_unquotedvalue_init(void);
bool hb_rule_attr_unquotedvalue_check(hb_rune c);

void hb_rule_entity_reference_map_add_entries(hb_map_entity_references* map);
void hb_rule_entity_reference_init(void);
bool hb_rule_entity_reference_valid_name_char(hb_rune c);
bool hb_rule_entity_reference_exists(nh_view_str* ref);
int32_t hb_rule_entity_reference_get_code_point(nh_view_str* ref);

void hb_rule_tag_content_add_elems(hb_set_tag_names* set);
void hb_rule_tag_content_init(void);
bool hb_rule_tag_content_check(nh_view_str* tag);

void hb_rule_tag_contentfirst_add_elems(hb_set_tag_names* set);
void hb_rule_tag_contentfirst_init(void);
bool hb_rule_tag_contentfirst_check(nh_view_str* tag);

void hb_rule_tag_formatting_add_elems(hb_set_tag_names* set);
void hb_rule_tag_formatting_init(void);
bool hb_rule_tag_formatting_check(nh_view_str* tag);

void hb_rule_tag_heading_add_elems(hb_set_tag_names* set);
void hb_rule_tag_heading_init(void);
bool hb_rule_tag_heading_check(nh_view_str* tag);

void hb_rule_tag_html_add_elems(hb_set_tag_names* set);
void hb_rule_tag_html_init(void);
bool hb_rule_tag_html_check(nh_view_str* tag);

void hb_rule_tag_layout_add_elems(hb_set_tag_names* set);
void hb_rule_tag_layout_init(void);
bool hb_rule_tag_layout_check(nh_view_str* tag);

void hb_rule_tag_media_add_elems(hb_set_tag_names* set);
void hb_rule_tag_media_init(void);
bool hb_rule_tag_media_check(nh_view_str* tag);

void hb_rule_tag_name_add_elems(nh_bitfield_ascii* set);
void hb_rule_tag_name_init(void);
bool hb_rule_tag_name_check(hb_rune c);

void hb_rule_tag_sectioning_add_elems(hb_set_tag_names* set);
void hb_rule_tag_sectioning_init(void);
bool hb_rule_tag_sectioning_check(nh_view_str* tag);

void hb_rule_tag_specific_add_elems(hb_set_tag_names* set);
void hb_rule_tag_specific_init(void);
bool hb_rule_tag_specific_check(nh_view_str* tag);

void hb_rule_tag_svg_add_elems(hb_set_tag_names* set);
void hb_rule_tag_svg_init(void);
bool hb_rule_tag_svg_check(nh_view_str* tag);

bool hb_rule_tag_valid_check(nh_view_str* tag);

void hb_rule_tag_void_add_elems(hb_set_tag_names* set);
void hb_rule_tag_void_init(void);
bool hb_rule_tag_void_check(nh_view_str* tag);

void hb_rule_tag_wss_add_elems(hb_set_tag_names* set);
void hb_rule_tag_wss_init(void);
bool hb_rule_tag_wss_check(nh_view_str* tag);

void hb_rule_tag_child_blacklist_map_add_entries(hb_map_tag_relations* map);
void hb_rule_tag_child_blacklist_init(void);
bool hb_rule_tag_child_blacklist_allowed(nh_view_str* parent, nh_view_str* child);

void hb_rule_tag_child_whitelist_map_add_entries(hb_map_tag_relations* map);
void hb_rule_tag_child_whitelist_init(void);
bool hb_rule_tag_child_whitelist_allowed(nh_view_str* parent, nh_view_str* child);

void hb_rule_tag_parent_blacklist_init(void);
bool hb_rule_tag_parent_blacklist_allowed(nh_view_str* child, nh_view_str* parent);

void hb_rule_tag_parent_whitelist_map_add_entries(hb_map_tag_relations* map);
void hb_rule_tag_parent_whitelist_init(void);
bool hb_rule_tag_parent_whitelist_allowed(nh_view_str* child, nh_view_str* parent);
