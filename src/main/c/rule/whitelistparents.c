#ifndef _HDR_HYPERBUILD_RULE_WHITELISTPARENTS
#define _HDR_HYPERBUILD_RULE_WHITELISTPARENTS

#include "../ext/nicehash/src/main/c/set/str.h"
#include "../ext/nicehash/src/main/c/map/str.h"

NICEHASH_MAP_STR(strset, nh_set_str_t)
static nh_map_str_strset_t hb_r_whitelistparents_map;

void hb_r_whitelistparents_init() {
  hb_r_whitelistparents_map = nh_map_str_strset_create();

  nh_set_str_t li = nh_set_str_create();
  nh_set_str_add(li, "ul");
  nh_set_str_add(li, "ol");
  nh_map_str_strset_set(hb_r_whitelistparents_map, "li", li);
}

int hb_r_whitelistparents_check(char *child) {
  return nh_map_str_strset_has(hb_r_whitelistparents_map, child);
}

int hb_r_whitelistparents_has(char *child, char *parent) {
  nh_map_str_strset_t set = nh_map_str_strset_get(hb_r_whitelistparents_map, child, NULL);
  return nh_set_str_has(set, parent);
}

#endif // _HDR_HYPERBUILD_RULE_WHITELISTPARENTS
