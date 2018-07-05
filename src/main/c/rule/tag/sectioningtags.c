#ifndef _HDR_HYPERBUILD_RULE_SECTIONINGTAGS
#define _HDR_HYPERBUILD_RULE_SECTIONINGTAGS

#include "../../ext/nicehash/set/str.h"
#include "../../util/hbchar.h"

static nh_set_str_t hb_r_sectioningtags_set;

void hb_r_sectioningtags_add_elems(nh_set_str_t set) {
  nh_set_str_add(set, "article");
  nh_set_str_add(set, "aside");
  nh_set_str_add(set, "nav");
  nh_set_str_add(set, "section");
}

void hb_r_sectioningtags_init(void) {
  hb_r_sectioningtags_set = nh_set_str_create();
  hb_r_sectioningtags_add_elems(hb_r_sectioningtags_set);
}

int hb_r_sectioningtags_check(hb_char_t *tag) {
  return nh_set_str_has(hb_r_sectioningtags_set, (char *) tag);
}

#endif // _HDR_HYPERBUILD_RULE_SECTIONINGTAGS
