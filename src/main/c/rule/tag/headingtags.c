#ifndef _HDR_HYPERBUILD_RULE_HEADINGTAGS
#define _HDR_HYPERBUILD_RULE_HEADINGTAGS

#include "../../ext/nicehash/set/str.h"
#include "../../util/hbchar.h"

static nh_set_str_t hbr_headingtags_set;

void hbr_headingtags_add_elems(nh_set_str_t set) {
  nh_set_str_add(set, "hgroup");
  nh_set_str_add(set, "h1");
  nh_set_str_add(set, "h2");
  nh_set_str_add(set, "h3");
  nh_set_str_add(set, "h4");
  nh_set_str_add(set, "h5");
  nh_set_str_add(set, "h6");
}

void hbr_headingtags_init(void) {
  hbr_headingtags_set = nh_set_str_create();
  hbr_headingtags_add_elems(hbr_headingtags_set);
}

int hbr_headingtags_check(hb_char_t *tag) {
  return nh_set_str_has(hbr_headingtags_set, (char *) tag);
}

#endif // _HDR_HYPERBUILD_RULE_HEADINGTAGS
