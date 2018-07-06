#ifndef _HDR_HYPERBUILD_RULE_SECTIONINGTAGS
#define _HDR_HYPERBUILD_RULE_SECTIONINGTAGS

#include "../../ext/nicehash/set/str.h"
#include "../../util/hbchar.h"

static nh_set_str_t hbr_sectioningtags_set;

void hbr_sectioningtags_add_elems(nh_set_str_t set) {
  nh_set_str_add(set, "article");
  nh_set_str_add(set, "aside");
  nh_set_str_add(set, "nav");
  nh_set_str_add(set, "section");
}

void hbr_sectioningtags_init(void) {
  hbr_sectioningtags_set = nh_set_str_create();
  hbr_sectioningtags_add_elems(hbr_sectioningtags_set);
}

int hbr_sectioningtags_check(hb_char_t *tag) {
  return nh_set_str_has(hbr_sectioningtags_set, (char *) tag);
}

#endif // _HDR_HYPERBUILD_RULE_SECTIONINGTAGS
