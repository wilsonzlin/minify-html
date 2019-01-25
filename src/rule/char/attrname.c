#include "./c0.c"

static nh_set_int32_t hb_rule_attrname_blacklist;

void hb_rule_attrname_init(void) {
  hb_rule_attrname_blacklist = nh_set_int32_create();
  hb_rule_c0_add_elems(hb_rule_attrname_blacklist);
  nh_set_int32_add(hb_rule_attrname_blacklist, ' ');
  nh_set_int32_add(hb_rule_attrname_blacklist, '"');
  nh_set_int32_add(hb_rule_attrname_blacklist, '\'');
  nh_set_int32_add(hb_rule_attrname_blacklist, '>');
  nh_set_int32_add(hb_rule_attrname_blacklist, '/');
  nh_set_int32_add(hb_rule_attrname_blacklist, '=');
  // NOTE: Unicode noncharacters not tested (https://html.spec.whatwg.org/multipage/syntax.html#syntax-attribute-name)
}

int hb_rule_attrname_check(hb_proc_char_t c) {
  return !nh_set_int32_has(hb_rule_attrname_blacklist, c);
}
