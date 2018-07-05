#ifndef _HDR_HYPERBUILD_RULE_BLACKLISTCHILDREN
#define _HDR_HYPERBUILD_RULE_BLACKLISTCHILDREN

#include "../../datastructure/map/str-strset.h"
#include "../../util/hbchar.h"

static nh_map_str_strset_t hb_r_blacklistchildren_map;

void hb_r_blacklistchildren_init(void) {
  hb_r_blacklistchildren_map = nh_map_str_strset_create();
}

int hb_r_blacklistchildren_check(hb_char_t *parent) {
  return nh_map_str_strset_has(hb_r_blacklistchildren_map, (char *) parent);
}

int hb_r_blacklistchildren_has(hb_char_t *parent, hb_char_t *child) {
  nh_set_str_t set = nh_map_str_strset_get(hb_r_blacklistchildren_map, (char *) parent, NULL);
  return nh_set_str_has(set, (char *) child);
}

#endif // _HDR_HYPERBUILD_RULE_BLACKLISTCHILDREN
