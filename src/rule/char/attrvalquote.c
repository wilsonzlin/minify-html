#include "./c0.c"

static nh_set_int32_t hb_rule_attrvalquote_set;

void hb_rule_attrvalquote_init(void) {
  hb_rule_attrvalquote_set = nh_set_int32_create();
  nh_set_int32_add(hb_rule_attrvalquote_set, '\'');
  nh_set_int32_add(hb_rule_attrvalquote_set, '"');
}

int hb_rule_attrvalquote_check(hb_proc_char_t c) {
  return nh_set_int32_has(hb_rule_attrvalquote_set, c);
}
