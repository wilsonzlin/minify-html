#ifndef _HDR_HYPERBUILD_RULE_WHITESPACE
#define _HDR_HYPERBUILD_RULE_WHITESPACE

#include "../../ext/nicehash/src/main/c/set/int32.h"

static nh_set_int32_t hb_r_whitespace_set;

void hb_r_whitespace_add_elems(nh_set_int32_t set) {
  nh_set_int32_add(set, 0x09); // TAB
  nh_set_int32_add(set, 0x0A); // LF
  nh_set_int32_add(set, 0x0C); // FF
  nh_set_int32_add(set, 0x0D); // CR
  nh_set_int32_add(set, 0x20); // SPACE
}

void hb_r_whitespace_init(void) {
  hb_r_whitespace_set = nh_set_int32_create();
  hb_r_whitespace_add_elems(hb_r_whitespace_set);
}

int hb_r_whitespace_check(char c) {
  return nh_set_int32_has(hb_r_whitespace_set, c);
}

#endif // _HDR_HYPERBUILD_RULE_WHITESPACE
