#ifndef _HDR_HYPERBUILD_RULE_UCALPHA
#define _HDR_HYPERBUILD_RULE_UCALPHA

#include "../../ext/nicehash/src/main/c/set/int32.h"

static nh_set_int32_t hb_r_ucalpha_set;

void hb_r_ucalpha_add_elems(nh_set_int32_t set) {
  for (char c = 0x41; c <= 0x5A; c++) {
    nh_set_int32_add(set, c);
  }
}

void hb_r_ucalpha_init(void) {
  hb_r_ucalpha_set = nh_set_int32_create();
  hb_r_ucalpha_add_elems(hb_r_ucalpha_set);
}

int hb_r_ucalpha_check(char c) {
  return nh_set_int32_has(hb_r_ucalpha_set, c);
}

#endif // _HDR_HYPERBUILD_RULE_UCALPHA
