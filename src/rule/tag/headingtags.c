static nh_set_str_t hb_rule_headingtags_set;

void hb_rule_headingtags_add_elems(nh_set_str_t set) {
  nh_set_str_add(set, "hgroup");
  nh_set_str_add(set, "h1");
  nh_set_str_add(set, "h2");
  nh_set_str_add(set, "h3");
  nh_set_str_add(set, "h4");
  nh_set_str_add(set, "h5");
  nh_set_str_add(set, "h6");
}

void hb_rule_headingtags_init(void) {
  hb_rule_headingtags_set = nh_set_str_create();
  hb_rule_headingtags_add_elems(hb_rule_headingtags_set);
}

int hb_rule_headingtags_check(hb_proc_char_t *tag) {
  return nh_set_str_has(hb_rule_headingtags_set, (char *) tag);
}
