#ifndef _HDR_HYPERBUILD_RULE_LAYOUTTAGS
#define _HDR_HYPERBUILD_RULE_LAYOUTTAGS

#include "../../ext/nicehash/set/str.h"
#include "../../util/hbchar.h"
#include "./sectioningtags.c"

static nh_set_str_t hbr_layouttags_set;

void hbr_layouttags_add_elems(nh_set_str_t set) {
  hbr_sectioningtags_add_elems(set);
  nh_set_str_add(set, "blockquote");
  nh_set_str_add(set, "body");
  nh_set_str_add(set, "colgroup");
  nh_set_str_add(set, "datalist");
  nh_set_str_add(set, "dialog");
  nh_set_str_add(set, "div");
  nh_set_str_add(set, "dl");
  nh_set_str_add(set, "fieldset");
  nh_set_str_add(set, "figure");
  nh_set_str_add(set, "footer");
  nh_set_str_add(set, "form");
  nh_set_str_add(set, "head");
  nh_set_str_add(set, "header");
  nh_set_str_add(set, "hgroup");
  nh_set_str_add(set, "html");
  nh_set_str_add(set, "main");
  nh_set_str_add(set, "map");
  nh_set_str_add(set, "menu");
  nh_set_str_add(set, "nav");
  nh_set_str_add(set, "ol");
  nh_set_str_add(set, "optgroup");
  nh_set_str_add(set, "picture");
  nh_set_str_add(set, "section");
  nh_set_str_add(set, "select");
  nh_set_str_add(set, "table");
  nh_set_str_add(set, "tbody");
  nh_set_str_add(set, "tfoot");
  nh_set_str_add(set, "thead");
  nh_set_str_add(set, "tr");
  nh_set_str_add(set, "ul");
}

void hbr_layouttags_init(void) {
  hbr_layouttags_set = nh_set_str_create();
  hbr_layouttags_add_elems(hbr_layouttags_set);
}

int hbr_layouttags_check(hb_char_t *tag) {
  return nh_set_str_has(hbr_layouttags_set, (char *) tag);
}

#endif // _HDR_HYPERBUILD_RULE_LAYOUTTAGS
