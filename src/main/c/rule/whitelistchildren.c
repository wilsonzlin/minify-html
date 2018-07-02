#ifndef _HDR_HYPERBUILD_RULE_WHITELISTCHILDREN
#define _HDR_HYPERBUILD_RULE_WHITELISTCHILDREN

#include "../ext/nicehash/src/main/c/set/str.h"
#include "../ext/nicehash/src/main/c/map/str.h"

NICEHASH_MAP_STR(strset, nh_set_str_t)
static nh_map_str_strset_t hb_r_whitelistchildren_map;

void hb_r_whitelistchildren_init() {
  hb_r_whitelistchildren_map = nh_map_str_strset_create();

  nh_set_str_t table = nh_set_str_create();
  nh_set_str_add(table, "caption");
  nh_set_str_add(table, "colgroup");
  nh_set_str_add(table, "thead");
  nh_set_str_add(table, "tbody");
  nh_set_str_add(table, "tfoot");
  nh_set_str_add(table, "tr");
  nh_map_str_strset_set(hb_r_whitelistchildren_map, "table", table);
}

int hb_r_whitelistchildren_check(char *parent) {
  return nh_map_str_strset_has(hb_r_whitelistchildren_map, parent);
}

int hb_r_whitelistchildren_has(char *parent, char *child) {
  nh_map_str_strset_t set = nh_map_str_strset_get(hb_r_whitelistchildren_map, parent, NULL);
  return nh_set_str_has(set, child);
}

#endif // _HDR_HYPERBUILD_RULE_WHITELISTCHILDREN
