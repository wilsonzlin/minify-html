#ifndef _HDR_HYPERBUILD_RULE_INLINETAGS
#define _HDR_HYPERBUILD_RULE_INLINETAGS

#include "../ext/nicehash/src/main/c/set/str.h"

static nh_set_str_t hb_r_inlinetags_set = nh_set_str_create();

void hb_r_inlinetags_init() {
  nh_set_str_add(hb_r_inlinetags_set, "a");
  nh_set_str_add(hb_r_inlinetags_set, "abbr");
  nh_set_str_add(hb_r_inlinetags_set, "b");
  nh_set_str_add(hb_r_inlinetags_set, "bdi");
  nh_set_str_add(hb_r_inlinetags_set, "bdo");
  nh_set_str_add(hb_r_inlinetags_set, "br");
  nh_set_str_add(hb_r_inlinetags_set, "cite");
  nh_set_str_add(hb_r_inlinetags_set, "code");
  nh_set_str_add(hb_r_inlinetags_set, "data");
  nh_set_str_add(hb_r_inlinetags_set, "dfn");
  nh_set_str_add(hb_r_inlinetags_set, "em");
  nh_set_str_add(hb_r_inlinetags_set, "i");
  nh_set_str_add(hb_r_inlinetags_set, "kbd");
  nh_set_str_add(hb_r_inlinetags_set, "mark");
  nh_set_str_add(hb_r_inlinetags_set, "q");
  nh_set_str_add(hb_r_inlinetags_set, "rp");
  nh_set_str_add(hb_r_inlinetags_set, "rt");
  nh_set_str_add(hb_r_inlinetags_set, "rtc");
  nh_set_str_add(hb_r_inlinetags_set, "ruby");
  nh_set_str_add(hb_r_inlinetags_set, "s");
  nh_set_str_add(hb_r_inlinetags_set, "samp");
  nh_set_str_add(hb_r_inlinetags_set, "small");
  nh_set_str_add(hb_r_inlinetags_set, "span");
  nh_set_str_add(hb_r_inlinetags_set, "strong");
  nh_set_str_add(hb_r_inlinetags_set, "sub");
  nh_set_str_add(hb_r_inlinetags_set, "sup");
  nh_set_str_add(hb_r_inlinetags_set, "time");
  nh_set_str_add(hb_r_inlinetags_set, "tt");
  nh_set_str_add(hb_r_inlinetags_set, "u");
  nh_set_str_add(hb_r_inlinetags_set, "var");
  nh_set_str_add(hb_r_inlinetags_set, "wbr");
}

int hb_r_inlinetags_check(char *tag) {
  return nh_set_str_has(hb_r_inlinetags_set, tag);
}

#endif // _HDR_HYPERBUILD_RULE_INLINETAGS
