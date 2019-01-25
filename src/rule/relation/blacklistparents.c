static nh_map_str_strset_t hb_rule_blacklistparents_map;

void hb_rule_blacklistparents_init(void) {
  hb_rule_blacklistparents_map = nh_map_str_strset_create();
}

int hb_rule_blacklistparents_allowed(hb_proc_char_t *child, hb_proc_char_t *parent) {
  nh_set_str_t set = nh_map_str_strset_get(hb_rule_blacklistparents_map, (char *) child, NULL);
  return set == NULL || !nh_set_str_has(set, (char *) parent);
}
