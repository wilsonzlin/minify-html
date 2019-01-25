static nh_set_str_t hb_rule_wsstags_set;

void hb_rule_wsstags_add_elems(nh_set_str_t set) {
  nh_set_str_add(set, "code");
  nh_set_str_add(set, "pre");
}

void hb_rule_wsstags_init(void) {
  hb_rule_wsstags_set = nh_set_str_create();
  hb_rule_wsstags_add_elems(hb_rule_wsstags_set);
}

int hb_rule_wsstags_check(hb_proc_char_t *tag) {
  return nh_set_str_has(hb_rule_wsstags_set, (char *) tag);
}
