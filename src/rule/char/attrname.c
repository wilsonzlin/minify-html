#include "./c0.c"

static nh_set_int32_t hbr_attrname_blacklist;

void hbr_attrname_init(void) {
  hbr_attrname_blacklist = nh_set_int32_create();
  hbr_c0_add_elems(hbr_attrname_blacklist);
  nh_set_int32_add(hbr_attrname_blacklist, ' ');
  nh_set_int32_add(hbr_attrname_blacklist, '"');
  nh_set_int32_add(hbr_attrname_blacklist, '\'');
  nh_set_int32_add(hbr_attrname_blacklist, '>');
  nh_set_int32_add(hbr_attrname_blacklist, '/');
  nh_set_int32_add(hbr_attrname_blacklist, '=');
  // NOTE: Unicode noncharacters not tested (https://html.spec.whatwg.org/multipage/syntax.html#syntax-attribute-name)
}

int hbr_attrname_check(hb_char_t c) {
  return !nh_set_int32_has(hbr_attrname_blacklist, c);
}
