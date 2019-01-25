#include "./whitespace.c"

static nh_set_int32_t hb_rule_unquotedattrval_blacklist;

void hb_rule_unquotedattrval_init(void) {
  hb_rule_unquotedattrval_blacklist = nh_set_int32_create();
  hb_rule_whitespace_add_elems(hb_rule_unquotedattrval_blacklist);
  nh_set_int32_add(hb_rule_unquotedattrval_blacklist, '"');
  nh_set_int32_add(hb_rule_unquotedattrval_blacklist, '\'');
  nh_set_int32_add(hb_rule_unquotedattrval_blacklist, '`');
  nh_set_int32_add(hb_rule_unquotedattrval_blacklist, '=');
  nh_set_int32_add(hb_rule_unquotedattrval_blacklist, '<');
  nh_set_int32_add(hb_rule_unquotedattrval_blacklist, '>');
}

int hb_rule_unquotedattrval_check(hb_proc_char_t c) {
  return !nh_set_int32_has(hb_rule_unquotedattrval_blacklist, c);
}
