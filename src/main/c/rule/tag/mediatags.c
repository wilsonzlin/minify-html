#ifndef _HDR_HYPERBUILD_RULE_MEDIATAGS
#define _HDR_HYPERBUILD_RULE_MEDIATAGS

#include "../../ext/nicehash/set/str.h"
#include "../../util/hbchar.h"

static nh_set_str_t hb_r_mediatags_set;

void hb_r_mediatags_add_elems(nh_set_str_t set) {
  nh_set_str_add(set, "audio");
  nh_set_str_add(set, "video");
}

void hb_r_mediatags_init(void) {
  hb_r_mediatags_set = nh_set_str_create();
  hb_r_mediatags_add_elems(hb_r_mediatags_set);
}

int hb_r_mediatags_check(hb_char_t *tag) {
  return nh_set_str_has(hb_r_mediatags_set, (char *) tag);
}

#endif // _HDR_HYPERBUILD_RULE_MEDIATAGS
