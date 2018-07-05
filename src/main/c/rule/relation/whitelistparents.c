#ifndef _HDR_HYPERBUILD_RULE_WHITELISTPARENTS
#define _HDR_HYPERBUILD_RULE_WHITELISTPARENTS

#include "../../datastructure/map/str-strset.h"
#include "../../util/hbchar.h"

static nh_map_str_strset_t hb_r_whitelistparents_map;

void hb_r_whitelistparents_init(void) {
  hb_r_whitelistparents_map = nh_map_str_strset_create();

  nh_set_str_t li = nh_set_str_create();
  nh_set_str_add(li, "ul");
  nh_set_str_add(li, "ol");
  nh_map_str_strset_set(hb_r_whitelistparents_map, "li", li);

  nh_set_str_t dt = nh_set_str_create();
  nh_set_str_add(dt, "dl");
  nh_map_str_strset_set(hb_r_whitelistparents_map, "dt", dt);

  nh_set_str_t dd = nh_set_str_create();
  nh_set_str_add(dd, "dl");
  nh_map_str_strset_set(hb_r_whitelistparents_map, "dd", dd);

  nh_set_str_t caption = nh_set_str_create();
  nh_set_str_add(caption, "table");
  nh_map_str_strset_set(hb_r_whitelistparents_map, "caption", caption);

  nh_set_str_t colgroup = nh_set_str_create();
  nh_set_str_add(colgroup, "table");
  nh_map_str_strset_set(hb_r_whitelistparents_map, "colgroup", colgroup);

  nh_set_str_t col = nh_set_str_create();
  nh_set_str_add(col, "table");
  nh_map_str_strset_set(hb_r_whitelistparents_map, "col", col);
}

int hb_r_whitelistparents_check(hb_char_t *child) {
  return nh_map_str_strset_has(hb_r_whitelistparents_map, (char *) child);
}

int hb_r_whitelistparents_has(hb_char_t *child, hb_char_t *parent) {
  nh_set_str_t set = nh_map_str_strset_get(hb_r_whitelistparents_map, (char *) child, NULL);
  return nh_set_str_has(set, (char *) parent);
}

#endif // _HDR_HYPERBUILD_RULE_WHITELISTPARENTS
