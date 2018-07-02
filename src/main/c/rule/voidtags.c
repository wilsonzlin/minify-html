#ifndef _HDR_HYPERBUILD_RULE_VOIDTAGS
#define _HDR_HYPERBUILD_RULE_VOIDTAGS

#include "../ext/nicehash/src/main/c/set/str.h"

static nh_set_str_t hb_r_voidtags_set;

void hb_r_voidtags_init() {
  hb_r_voidtags_set = nh_set_str_create();
  nh_set_str_add(hb_r_voidtags_set, "area");
  nh_set_str_add(hb_r_voidtags_set, "base");
  nh_set_str_add(hb_r_voidtags_set, "br");
  nh_set_str_add(hb_r_voidtags_set, "col");
  nh_set_str_add(hb_r_voidtags_set, "embed");
  nh_set_str_add(hb_r_voidtags_set, "hr");
  nh_set_str_add(hb_r_voidtags_set, "img");
  nh_set_str_add(hb_r_voidtags_set, "input");
  nh_set_str_add(hb_r_voidtags_set, "keygen");
  nh_set_str_add(hb_r_voidtags_set, "link");
  nh_set_str_add(hb_r_voidtags_set, "meta");
  nh_set_str_add(hb_r_voidtags_set, "param");
  nh_set_str_add(hb_r_voidtags_set, "source");
  nh_set_str_add(hb_r_voidtags_set, "track");
  nh_set_str_add(hb_r_voidtags_set, "wbr");
}

int hb_r_voidtags_check(char *tag) {
  return nh_set_str_has(hb_r_voidtags_set, tag);
}

#endif // _HDR_HYPERBUILD_RULE_VOIDTAGS
