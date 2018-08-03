#ifndef _HDR_HYPERBUILD_RULE_HEX
#define _HDR_HYPERBUILD_RULE_HEX

#include "../../ext/nicehash/set/int32.h"
#include "../../util/hbchar.h"

static nh_set_int32_t hbr_hex_set;

void hbr_hex_add_elems(nh_set_int32_t set) {
  for (char c = 0x30; c <= 0x39; c++) { // 0-9
    nh_set_int32_add(set, c);
  }
  for (char c = 0x41; c <= 0x46; c++) { // A-F
    nh_set_int32_add(set, c);
  }
  for (char c = 0x61; c <= 0x66; c++) { // a-f
    nh_set_int32_add(set, c);
  }
}

void hbr_hex_init(void) {
  hbr_hex_set = nh_set_int32_create();
  hbr_hex_add_elems(hbr_hex_set);
}

int hbr_hex_check(hb_char_t c) {
  return nh_set_int32_has(hbr_hex_set, c);
}

#endif // _HDR_HYPERBUILD_RULE_HEX
