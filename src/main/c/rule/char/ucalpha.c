#ifndef _HDR_HYPERBUILD_RULE_UCALPHA
#define _HDR_HYPERBUILD_RULE_UCALPHA

#include "../../ext/nicehash/set/int32.h"
#include "../../util/hbchar.h"

static nh_set_int32_t hbr_ucalpha_set;

void hbr_ucalpha_add_elems(nh_set_int32_t set) {
  for (char c = 0x41; c <= 0x5A; c++) {
    nh_set_int32_add(set, c);
  }
}

void hbr_ucalpha_init(void) {
  hbr_ucalpha_set = nh_set_int32_create();
  hbr_ucalpha_add_elems(hbr_ucalpha_set);
}

int hbr_ucalpha_check(hb_char_t c) {
  return nh_set_int32_has(hbr_ucalpha_set, c);
}

#endif // _HDR_HYPERBUILD_RULE_UCALPHA
