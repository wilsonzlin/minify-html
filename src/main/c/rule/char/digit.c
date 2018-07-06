#ifndef _HDR_HYPERBUILD_RULE_DIGIT
#define _HDR_HYPERBUILD_RULE_DIGIT

#include "../../ext/nicehash/set/int32.h"
#include "../../util/hbchar.h"

static nh_set_int32_t hbr_digit_set;

void hbr_digit_add_elems(nh_set_int32_t set) {
  for (char c = 0x30; c <= 0x39; c++) {
    nh_set_int32_add(set, c);
  }
}

void hbr_digit_init(void) {
  hbr_digit_set = nh_set_int32_create();
  hbr_digit_add_elems(hbr_digit_set);
}

int hbr_digit_check(hb_char_t c) {
  return nh_set_int32_has(hbr_digit_set, c);
}

#endif // _HDR_HYPERBUILD_RULE_DIGIT
