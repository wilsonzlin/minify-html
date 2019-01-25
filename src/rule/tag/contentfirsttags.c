static nh_set_str_t hb_rule_contentfirsttags_set;

void hb_rule_contentfirsttags_add_elems(nh_set_str_t set) {
  nh_set_str_add(set, "dd");
  nh_set_str_add(set, "details");
  nh_set_str_add(set, "dt");
  nh_set_str_add(set, "iframe");
  nh_set_str_add(set, "label");
  nh_set_str_add(set, "li");
  nh_set_str_add(set, "noscript");
  nh_set_str_add(set, "output");
  nh_set_str_add(set, "progress");
  nh_set_str_add(set, "slot");
  nh_set_str_add(set, "td");
  nh_set_str_add(set, "template");
  nh_set_str_add(set, "th");
}

void hb_rule_contentfirsttags_init(void) {
  hb_rule_contentfirsttags_set = nh_set_str_create();
  hb_rule_contentfirsttags_add_elems(hb_rule_contentfirsttags_set);
}

int hb_rule_contentfirsttags_check(hb_proc_char_t *tag) {
  return nh_set_str_has(hb_rule_contentfirsttags_set, (char *) tag);
}
