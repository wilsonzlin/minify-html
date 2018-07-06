#ifndef _HDR_HYPERBUILD_RULE_VOIDTAGS
#define _HDR_HYPERBUILD_RULE_VOIDTAGS

#include "../../ext/nicehash/set/str.h"
#include "../../util/hbchar.h"

static nh_set_str_t hbr_voidtags_set;

void hbr_voidtags_add_elems(nh_set_str_t set) {
  nh_set_str_add(set, "area");
  nh_set_str_add(set, "base");
  nh_set_str_add(set, "br");
  nh_set_str_add(set, "col");
  nh_set_str_add(set, "embed");
  nh_set_str_add(set, "hr");
  nh_set_str_add(set, "img");
  nh_set_str_add(set, "input");
  nh_set_str_add(set, "keygen");
  nh_set_str_add(set, "link");
  nh_set_str_add(set, "meta");
  nh_set_str_add(set, "param");
  nh_set_str_add(set, "source");
  nh_set_str_add(set, "track");
  nh_set_str_add(set, "wbr");
}

void hbr_voidtags_init(void) {
  hbr_voidtags_set = nh_set_str_create();
  hbr_voidtags_add_elems(hbr_voidtags_set);
}

int hbr_voidtags_check(hb_char_t *tag) {
  return nh_set_str_has(hbr_voidtags_set, (char *) tag);
}

#endif // _HDR_HYPERBUILD_RULE_VOIDTAGS
