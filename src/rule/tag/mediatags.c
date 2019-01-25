static nh_set_str_t hb_rule_mediatags_set;

void hb_rule_mediatags_add_elems(nh_set_str_t set) {
  nh_set_str_add(set, "audio");
  nh_set_str_add(set, "video");
}

void hb_rule_mediatags_init(void) {
  hb_rule_mediatags_set = nh_set_str_create();
  hb_rule_mediatags_add_elems(hb_rule_mediatags_set);
}

int hb_rule_mediatags_check(hb_proc_char_t *tag) {
  return nh_set_str_has(hb_rule_mediatags_set, (char *) tag);
}
