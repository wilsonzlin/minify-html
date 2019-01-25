static nh_set_int32_t hb_rule_c0_set;

void hb_rule_c0_add_elems(nh_set_int32_t set) {
  for (char c = 0x0; c <= 0x1F; c++) {
    nh_set_int32_add(set, c);
  }
}

void hb_rule_c0_init(void) {
  hb_rule_c0_set = nh_set_int32_create();
  hb_rule_c0_add_elems(hb_rule_c0_set);
}

int hb_rule_c0_check(hb_proc_char_t c) {
  return nh_set_int32_has(hb_rule_c0_set, c);
}
