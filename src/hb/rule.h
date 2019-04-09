#pragma once

#include <hb/rune.h>
#include <hb/collection.h>

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

void hb_rule_tag_name_add_elems(nh_set_int32* set);
void hb_rule_tag_name_init(void);
bool hb_rule_tag_name_check(hb_rune c);

void hb_rule_entity_reference_map_add_entries(nh_map_str_int32* map);
void hb_rule_entity_reference_map_init(void);
bool hb_rule_entity_reference_map_check(hb_rune* ref);
int32_t hb_rule_entity_reference_map_get(hb_rune* ref);
