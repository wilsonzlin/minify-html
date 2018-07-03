#ifndef _HDR_HYPERBUILD_RULE_ATTRNAME
#define _HDR_HYPERBUILD_RULE_ATTRNAME

#include "../../ext/nicehash/src/main/c/set/int32.h"
#include "./c0.c"

static nh_set_int32_t hb_r_attrname_blacklist;

void hb_r_attrname_init() {
  hb_r_attrname_blacklist = nh_set_int32_create();
  hb_r_c0_add_elems(hb_r_attrname_blacklist);
  nh_set_int32_add(hb_r_attrname_blacklist, ' ');
  nh_set_int32_add(hb_r_attrname_blacklist, '"');
  nh_set_int32_add(hb_r_attrname_blacklist, '\'');
  nh_set_int32_add(hb_r_attrname_blacklist, '>');
  nh_set_int32_add(hb_r_attrname_blacklist, '/');
  nh_set_int32_add(hb_r_attrname_blacklist, '=');
  // NOTE: Unicode noncharacters not tested (https://html.spec.whatwg.org/multipage/syntax.html#syntax-attribute-name)
}

int hb_r_attrname_check(char c) {
  return !nh_set_int32_has(hb_r_attrname_blacklist, c);
}

#endif // _HDR_HYPERBUILD_RULE_ATTRNAME
