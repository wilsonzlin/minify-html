#ifndef _HDR_HYPERBUILD_RULE_BLACKLISTPARENTS
#define _HDR_HYPERBUILD_RULE_BLACKLISTPARENTS

#include "../../datastructure/map/str-strset.h"
#include "../../util/hbchar.h"

static nh_map_str_strset_t hbr_blacklistparents_map;

void hbr_blacklistparents_init(void) {
  hbr_blacklistparents_map = nh_map_str_strset_create();
}

int hbr_blacklistparents_allowed(hb_char_t *child, hb_char_t *parent) {
  nh_set_str_t set = nh_map_str_strset_get(hbr_blacklistparents_map, (char *) child, NULL);
  return set == NULL || !nh_set_str_has(set, (char *) parent);
}

#endif // _HDR_HYPERBUILD_RULE_BLACKLISTPARENTS
