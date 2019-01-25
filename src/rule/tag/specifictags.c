#include "./sectioningtags.c"
#include "./svgtags.c"

static nh_set_str_t hb_rule_specifictags_set;

void hb_rule_specifictags_add_elems(nh_set_str_t set) {
  hb_rule_svgtags_add_elems(set);
  nh_set_str_add(set, "area");
  nh_set_str_add(set, "base");
  nh_set_str_add(set, "br");
  nh_set_str_add(set, "code"); // Reason: unlikely to want to minify
  nh_set_str_add(set, "col");
  nh_set_str_add(set, "embed");
  nh_set_str_add(set, "hr");
  nh_set_str_add(set, "img");
  nh_set_str_add(set, "input");
  nh_set_str_add(set, "param");
  nh_set_str_add(set, "pre"); // Reason: unlikely to want to minify
  nh_set_str_add(set, "script");
  nh_set_str_add(set, "source");
  nh_set_str_add(set, "track");
}

void hb_rule_specifictags_init(void) {
  hb_rule_specifictags_set = nh_set_str_create();
  hb_rule_specifictags_add_elems(hb_rule_specifictags_set);
}

int hb_rule_specifictags_check(hb_proc_char_t *tag) {
  return nh_set_str_has(hb_rule_specifictags_set, (char *) tag);
}
