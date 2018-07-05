#ifndef _HDR_HYPERBUILD_RULE_WHITELISTCHILDREN
#define _HDR_HYPERBUILD_RULE_WHITELISTCHILDREN

#include "../../datastructure/map/str-strset.h"
#include "../../util/hbchar.h"

static nh_map_str_strset_t hb_r_whitelistchildren_map;

void hb_r_whitelistchildren_init(void) {
  hb_r_whitelistchildren_map = nh_map_str_strset_create();

  nh_set_str_t table = nh_set_str_create();
  nh_set_str_add(table, "caption");
  nh_set_str_add(table, "colgroup");
  nh_set_str_add(table, "col");
  nh_set_str_add(table, "thead");
  nh_set_str_add(table, "tbody");
  nh_set_str_add(table, "tfoot");
  nh_set_str_add(table, "tr");
  nh_map_str_strset_set(hb_r_whitelistchildren_map, "table", table);
}

int hb_r_whitelistchildren_check(hb_char_t *parent) {
  return nh_map_str_strset_has(hb_r_whitelistchildren_map, (char *) parent);
}

int hb_r_whitelistchildren_has(hb_char_t *parent, hb_char_t *child) {
  nh_set_str_t set = nh_map_str_strset_get(hb_r_whitelistchildren_map, (char *) parent, NULL);
  return nh_set_str_has(set, (char *) child);
}

#endif // _HDR_HYPERBUILD_RULE_WHITELISTCHILDREN
