#ifndef _HDR_HYPERBUILD_RULE_BLACKLISTCHILDREN
#define _HDR_HYPERBUILD_RULE_BLACKLISTCHILDREN

#include "../../datastructure/map/str-strset.h"

static nh_map_str_strset_t hb_r_blacklistchildren_map;

void hb_r_blacklistchildren_init(void) {
  hb_r_blacklistchildren_map = nh_map_str_strset_create();
}

int hb_r_blacklistchildren_check(char *parent) {
  return nh_map_str_strset_has(hb_r_blacklistchildren_map, parent);
}

int hb_r_blacklistchildren_has(char *parent, char *child) {
  nh_map_str_strset_t set = nh_map_str_strset_get(hb_r_blacklistchildren_map, parent, NULL);
  return nh_set_str_has(set, child);
}

#endif // _HDR_HYPERBUILD_RULE_BLACKLISTCHILDREN
