#ifndef _HDR_HYPERBUILD_RULE_ATTRVALQUOTE
#define _HDR_HYPERBUILD_RULE_ATTRVALQUOTE

#include "../../ext/nicehash/set/int32.h"
#include "../../util/hbchar.h"
#include "./c0.c"

static nh_set_int32_t hb_r_attrvalquote_set;

void hb_r_attrvalquote_init(void) {
  hb_r_attrvalquote_set = nh_set_int32_create();
  nh_set_int32_add(hb_r_attrvalquote_set, '\'');
  nh_set_int32_add(hb_r_attrvalquote_set, '"');
}

int hb_r_attrvalquote_check(hb_char_t c) {
  return !nh_set_int32_has(hb_r_attrvalquote_set, c);
}

#endif // _HDR_HYPERBUILD_RULE_ATTRVALQUOTE
