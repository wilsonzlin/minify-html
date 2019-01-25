static nh_set_int32_t hb_rule_hex_set;

void hb_rule_hex_add_elems(nh_set_int32_t set) {
  for (char c = 0x30; c <= 0x39; c++) { // 0-9
    nh_set_int32_add(set, c);
  }
  for (char c = 0x41; c <= 0x46; c++) { // A-F
    nh_set_int32_add(set, c);
  }
  for (char c = 0x61; c <= 0x66; c++) { // a-f
    nh_set_int32_add(set, c);
  }
}

void hb_rule_hex_init(void) {
  hb_rule_hex_set = nh_set_int32_create();
  hb_rule_hex_add_elems(hb_rule_hex_set);
}

int hb_rule_hex_check(hb_proc_char_t c) {
  return nh_set_int32_has(hb_rule_hex_set, c);
}
