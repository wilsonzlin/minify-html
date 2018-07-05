#ifndef _HDR_HYPERBUILD_RULE_DIGIT
#define _HDR_HYPERBUILD_RULE_DIGIT

#include "../../ext/nicehash/set/int32.h"

static nh_set_int32_t hb_r_digit_set;

void hb_r_digit_add_elems(nh_set_int32_t set) {
  for (char c = 0x30; c <= 0x39; c++) {
    nh_set_int32_add(set, c);
  }
}

void hb_r_digit_init(void) {
  hb_r_digit_set = nh_set_int32_create();
  hb_r_digit_add_elems(hb_r_digit_set);
}

int hb_r_digit_check(char c) {
  return nh_set_int32_has(hb_r_digit_set, c);
}

#endif // _HDR_HYPERBUILD_RULE_DIGIT
