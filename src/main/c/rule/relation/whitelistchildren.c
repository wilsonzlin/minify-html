#ifndef _HDR_HYPERBUILD_RULE_WHITELISTCHILDREN
#define _HDR_HYPERBUILD_RULE_WHITELISTCHILDREN

#include "../../datastructure/map/str-strset.h"
#include "../../util/hbchar.h"
#include "../tag/headingtags.c"

static nh_map_str_strset_t hb_r_whitelistchildren_map;

void hb_r_whitelistchildren_init(void) {
  hb_r_whitelistchildren_map = nh_map_str_strset_create();

  // <colgroup>
  nh_set_str_t colgroup = nh_set_str_create();
  nh_set_str_add(colgroup, "col");
  nh_map_str_strset_set(hb_r_whitelistchildren_map, "colgroup", colgroup);

  // <datalist>
  nh_set_str_t datalist = nh_set_str_create();
  nh_set_str_add(datalist, "option");
  nh_map_str_strset_set(hb_r_whitelistchildren_map, "datalist", datalist);

  // <dl>
  nh_set_str_t dl = nh_set_str_create();
  nh_set_str_add(dl, "dt");
  nh_set_str_add(dl, "dd");
  nh_map_str_strset_set(hb_r_whitelistchildren_map, "dl", dl);

  // <hgroup>
  nh_set_str_t hgroup = nh_set_str_create();
  hb_r_headingtags_add_elems(hgroup);
  nh_map_str_strset_set(hb_r_whitelistchildren_map, "hgroup", hgroup);

  // <ol>
  nh_set_str_t ol = nh_set_str_create();
  nh_set_str_add(ol, "li");
  nh_map_str_strset_set(hb_r_whitelistchildren_map, "ol", ol);

  // <optgroup>
  nh_set_str_t optgroup = nh_set_str_create();
  nh_set_str_add(optgroup, "option");
  nh_map_str_strset_set(hb_r_whitelistchildren_map, "optgroup", optgroup);

  // <picture>
  nh_set_str_t picture = nh_set_str_create();
  nh_set_str_add(picture, "source");
  nh_set_str_add(picture, "img");
  nh_map_str_strset_set(hb_r_whitelistchildren_map, "picture", picture);

  // <select>
  nh_set_str_t select = nh_set_str_create();
  nh_set_str_add(select, "optgroup");
  nh_set_str_add(select, "option");
  nh_map_str_strset_set(hb_r_whitelistchildren_map, "select", select);

  // <table>
  nh_set_str_t table = nh_set_str_create();
  nh_set_str_add(table, "caption");
  nh_set_str_add(table, "colgroup");
  nh_set_str_add(table, "col");
  nh_set_str_add(table, "thead");
  nh_set_str_add(table, "tbody");
  nh_set_str_add(table, "tfoot");
  nh_set_str_add(table, "tr");
  nh_map_str_strset_set(hb_r_whitelistchildren_map, "table", table);

  // <tbody>
  nh_set_str_t tbody = nh_set_str_create();
  nh_set_str_add(tbody, "tr");
  nh_map_str_strset_set(hb_r_whitelistchildren_map, "tbody", tbody);

  // <tfoot>
  nh_set_str_t tfoot = nh_set_str_create();
  nh_set_str_add(tfoot, "tr");
  nh_map_str_strset_set(hb_r_whitelistchildren_map, "tfoot", tfoot);

  // <thead>
  nh_set_str_t thead = nh_set_str_create();
  nh_set_str_add(thead, "tr");
  nh_map_str_strset_set(hb_r_whitelistchildren_map, "thead", thead);

  // <tr>
  nh_set_str_t tr = nh_set_str_create();
  nh_set_str_add(tr, "td");
  nh_set_str_add(tr, "th");
  nh_set_str_add(tr, "template");
  nh_set_str_add(tr, "script");
  nh_map_str_strset_set(hb_r_whitelistchildren_map, "tr", tr);

  // <ul>
  nh_set_str_t ul = nh_set_str_create();
  nh_set_str_add(ul, "li");
  nh_map_str_strset_set(hb_r_whitelistchildren_map, "ul", ul);
}

int hb_r_whitelistchildren_check(hb_char_t *parent) {
  return nh_map_str_strset_has(hb_r_whitelistchildren_map, (char *) parent);
}

int hb_r_whitelistchildren_has(hb_char_t *parent, hb_char_t *child) {
  nh_set_str_t set = nh_map_str_strset_get(hb_r_whitelistchildren_map, (char *) parent, NULL);
  return nh_set_str_has(set, (char *) child);
}

#endif // _HDR_HYPERBUILD_RULE_WHITELISTCHILDREN
