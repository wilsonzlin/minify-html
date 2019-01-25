static nh_set_str_t hb_rule_sectioningtags_set;

void hb_rule_sectioningtags_add_elems(nh_set_str_t set) {
  nh_set_str_add(set, "article");
  nh_set_str_add(set, "aside");
  nh_set_str_add(set, "nav");
  nh_set_str_add(set, "section");
}

void hb_rule_sectioningtags_init(void) {
  hb_rule_sectioningtags_set = nh_set_str_create();
  hb_rule_sectioningtags_add_elems(hb_rule_sectioningtags_set);
}

int hb_rule_sectioningtags_check(hb_proc_char_t *tag) {
  return nh_set_str_has(hb_rule_sectioningtags_set, (char *) tag);
}
