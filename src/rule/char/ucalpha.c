static nh_set_int32_t hb_rule_ucalpha_set;

void hb_rule_ucalpha_add_elems(nh_set_int32_t set) {
  for (char c = 0x41; c <= 0x5A; c++) {
    nh_set_int32_add(set, c);
  }
}

void hb_rule_ucalpha_init(void) {
  hb_rule_ucalpha_set = nh_set_int32_create();
  hb_rule_ucalpha_add_elems(hb_rule_ucalpha_set);
}

int hb_rule_ucalpha_check(hb_proc_char_t c) {
  return nh_set_int32_has(hb_rule_ucalpha_set, c);
}
