#ifndef _HDR_HYPERBUILD_RULE_TAGNAME
#define _HDR_HYPERBUILD_RULE_TAGNAME

#include "../../ext/nicehash/set/int32.h"
#include "../../util/hbchar.h"
#include "./lcalpha.c"
#include "./ucalpha.c"
#include "./digit.c"

static nh_set_int32_t hb_r_tagname_set;

void hb_r_tagname_init(void) {
  hb_r_tagname_set = nh_set_int32_create();
  hb_r_lcalpha_add_elems(hb_r_tagname_set);
  hb_r_ucalpha_add_elems(hb_r_tagname_set);
  hb_r_digit_add_elems(hb_r_tagname_set);
}

int hb_r_tagname_check(hb_char_t c) {
  return nh_set_int32_has(hb_r_tagname_set, c);
}

#endif // _HDR_HYPERBUILD_RULE_TAGNAME
