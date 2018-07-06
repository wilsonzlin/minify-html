#ifndef _HDR_HYPERBUILD_RULE_SPECIFICTAGS
#define _HDR_HYPERBUILD_RULE_SPECIFICTAGS

#include "../../ext/nicehash/set/str.h"
#include "../../util/hbchar.h"
#include "./sectioningtags.c"

static nh_set_str_t hbr_specifictags_set;

void hbr_specifictags_add_elems(nh_set_str_t set) {
  nh_set_str_add(set, "area");
  nh_set_str_add(set, "br");
  nh_set_str_add(set, "col");
  nh_set_str_add(set, "embed");
  nh_set_str_add(set, "hr");
  nh_set_str_add(set, "img");
  nh_set_str_add(set, "input");
  nh_set_str_add(set, "param");
  nh_set_str_add(set, "pre"); // Reason: unlikely to want to minify
  nh_set_str_add(set, "script");
  nh_set_str_add(set, "source");
  nh_set_str_add(set, "track");
}

void hbr_specifictags_init(void) {
  hbr_specifictags_set = nh_set_str_create();
  hbr_specifictags_add_elems(hbr_specifictags_set);
}

int hbr_specifictags_check(hb_char_t *tag) {
  return nh_set_str_has(hbr_specifictags_set, (char *) tag);
}

#endif // _HDR_HYPERBUILD_RULE_SPECIFICTAGS
