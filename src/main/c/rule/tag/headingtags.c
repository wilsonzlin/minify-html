#ifndef _HDR_HYPERBUILD_RULE_HEADINGTAGS
#define _HDR_HYPERBUILD_RULE_HEADINGTAGS

#include "../../ext/nicehash/set/str.h"
#include "../../util/hbchar.h"

static nh_set_str_t hb_r_headingtags_set;

void hb_r_headingtags_add_elems(nh_set_str_t set) {
  nh_set_str_add(set, "hgroup");
  nh_set_str_add(set, "h1");
  nh_set_str_add(set, "h2");
  nh_set_str_add(set, "h3");
  nh_set_str_add(set, "h4");
  nh_set_str_add(set, "h5");
  nh_set_str_add(set, "h6");
}

void hb_r_headingtags_init(void) {
  hb_r_headingtags_set = nh_set_str_create();
  hb_r_headingtags_add_elems(hb_r_headingtags_set);
}

int hb_r_headingtags_check(hb_char_t *tag) {
  return nh_set_str_has(hb_r_headingtags_set, (char *) tag);
}

#endif // _HDR_HYPERBUILD_RULE_HEADINGTAGS
