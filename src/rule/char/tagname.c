#include "./lcalpha.c"
#include "./ucalpha.c"
#include "./digit.c"

static nh_set_int32_t hb_rule_tagname_set;

void hb_rule_tagname_init(void) {
  hb_rule_tagname_set = nh_set_int32_create();
  hb_rule_lcalpha_add_elems(hb_rule_tagname_set);
  hb_rule_ucalpha_add_elems(hb_rule_tagname_set);
  hb_rule_digit_add_elems(hb_rule_tagname_set);
  nh_set_int32_add(hb_rule_tagname_set, ':');
  nh_set_int32_add(hb_rule_tagname_set, '-');
}

int hb_rule_tagname_check(hb_proc_char_t c) {
  return nh_set_int32_has(hb_rule_tagname_set, c);
}
