#ifndef _HDR_HYPERBUILD_RULE_BLACKLISTPARENTS
#define _HDR_HYPERBUILD_RULE_BLACKLISTPARENTS

#include "../../ext/nicehash/src/main/c/set/str.h"
#include "../../ext/nicehash/src/main/c/map/str.h"

NICEHASH_MAP_STR(strset, nh_set_str_t)
static nh_map_str_strset_t hb_r_blacklistparents_map;

void hb_r_blacklistparents_init(void) {
  hb_r_blacklistparents_map = nh_map_str_strset_create();
}

int hb_r_blacklistparents_check(char *child) {
  return nh_map_str_strset_has(hb_r_blacklistparents_map, child);
}

int hb_r_blacklistparents_has(char *child, char *parent) {
  nh_map_str_strset_t set = nh_map_str_strset_get(hb_r_blacklistparents_map, child, NULL);
  return nh_set_str_has(set, parent);
}

#endif // _HDR_HYPERBUILD_RULE_BLACKLISTPARENTS
