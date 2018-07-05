#ifndef _HDR_HYPERBUILD_RULE_BLACKLISTPARENTS
#define _HDR_HYPERBUILD_RULE_BLACKLISTPARENTS

#include "../../datastructure/map/str-strset.h"
#include "../../util/hbchar.h"

static nh_map_str_strset_t hb_r_blacklistparents_map;

void hb_r_blacklistparents_init(void) {
  hb_r_blacklistparents_map = nh_map_str_strset_create();
}

int hb_r_blacklistparents_check(hb_char_t *child) {
  return nh_map_str_strset_has(hb_r_blacklistparents_map, (char *) child);
}

int hb_r_blacklistparents_has(hb_char_t *child, hb_char_t *parent) {
  nh_set_str_t set = nh_map_str_strset_get(hb_r_blacklistparents_map, (char *) child, NULL);
  return nh_set_str_has(set, (char *) parent);
}

#endif // _HDR_HYPERBUILD_RULE_BLACKLISTPARENTS
