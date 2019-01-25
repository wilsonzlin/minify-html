static nh_set_int32_t hb_rule_whitespace_set;

void hb_rule_whitespace_add_elems(nh_set_int32_t set) {
  nh_set_int32_add(set, 0x09); // TAB
  nh_set_int32_add(set, 0x0A); // LF
  nh_set_int32_add(set, 0x0C); // FF
  nh_set_int32_add(set, 0x0D); // CR
  nh_set_int32_add(set, 0x20); // SPACE
}

void hb_rule_whitespace_init(void) {
  hb_rule_whitespace_set = nh_set_int32_create();
  hb_rule_whitespace_add_elems(hb_rule_whitespace_set);
}

int hb_rule_whitespace_check(hb_proc_char_t c) {
  return nh_set_int32_has(hb_rule_whitespace_set, c);
}
