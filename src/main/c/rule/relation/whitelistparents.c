#ifndef _HDR_HYPERBUILD_RULE_WHITELISTPARENTS
#define _HDR_HYPERBUILD_RULE_WHITELISTPARENTS

#include "../../datastructure/map/str-strset.h"
#include "../../util/hbchar.h"
#include "../tag/mediatags.c"

static nh_map_str_strset_t hbr_whitelistparents_map;

void hbr_whitelistparents_init(void) {
  hbr_whitelistparents_map = nh_map_str_strset_create();

  // <caption>
  nh_set_str_t caption = nh_set_str_create();
  nh_set_str_add(caption, "table");
  nh_map_str_strset_set(hbr_whitelistparents_map, "caption", caption);

  // <col>
  nh_set_str_t col = nh_set_str_create();
  nh_set_str_add(col, "table");
  nh_set_str_add(col, "colgroup");
  nh_map_str_strset_set(hbr_whitelistparents_map, "col", col);

  // <colgroup>
  nh_set_str_t colgroup = nh_set_str_create();
  nh_set_str_add(colgroup, "table");
  nh_map_str_strset_set(hbr_whitelistparents_map, "colgroup", colgroup);

  // <dd>
  nh_set_str_t dd = nh_set_str_create();
  nh_set_str_add(dd, "dl");
  nh_map_str_strset_set(hbr_whitelistparents_map, "dd", dd);

  // <dt>
  nh_set_str_t dt = nh_set_str_create();
  nh_set_str_add(dt, "dl");
  nh_map_str_strset_set(hbr_whitelistparents_map, "dt", dt);

  // <figcaption>
  nh_set_str_t figcaption = nh_set_str_create();
  nh_set_str_add(figcaption, "figure");
  nh_map_str_strset_set(hbr_whitelistparents_map, "figcaption", figcaption);

  // <legend>
  nh_set_str_t legend = nh_set_str_create();
  nh_set_str_add(legend, "fieldset");
  nh_map_str_strset_set(hbr_whitelistparents_map, "legend", legend);

  // <li>
  nh_set_str_t li = nh_set_str_create();
  nh_set_str_add(li, "ul");
  nh_set_str_add(li, "ol");
  nh_set_str_add(li, "menu");
  nh_map_str_strset_set(hbr_whitelistparents_map, "li", li);

  // <optgroup>
  nh_set_str_t optgroup = nh_set_str_create();
  nh_set_str_add(optgroup, "select");
  nh_map_str_strset_set(hbr_whitelistparents_map, "optgroup", optgroup);

  // <option>
  nh_set_str_t option = nh_set_str_create();
  nh_set_str_add(option, "select");
  nh_set_str_add(option, "optgroup");
  nh_set_str_add(option, "datalist");
  nh_map_str_strset_set(hbr_whitelistparents_map, "option", option);

  // <param>
  nh_set_str_t param = nh_set_str_create();
  nh_set_str_add(param, "object");
  nh_map_str_strset_set(hbr_whitelistparents_map, "param", param);

  // <rp>
  nh_set_str_t rp = nh_set_str_create();
  nh_set_str_add(rp, "ruby");
  nh_map_str_strset_set(hbr_whitelistparents_map, "rp", rp);

  // <rt>
  nh_set_str_t rt = nh_set_str_create();
  nh_set_str_add(rt, "ruby");
  nh_map_str_strset_set(hbr_whitelistparents_map, "rt", rt);

  // <rtc>
  nh_set_str_t rtc = nh_set_str_create();
  nh_set_str_add(rtc, "ruby");
  nh_map_str_strset_set(hbr_whitelistparents_map, "rtc", rtc);

  // <summary>
  nh_set_str_t summary = nh_set_str_create();
  nh_set_str_add(summary, "details");
  nh_map_str_strset_set(hbr_whitelistparents_map, "summary", summary);

  // <source>
  nh_set_str_t source = nh_set_str_create();
  hbr_mediatags_add_elems(source);
  nh_set_str_add(source, "picture");
  nh_map_str_strset_set(hbr_whitelistparents_map, "source", source);

  // <track>
  nh_set_str_t track = nh_set_str_create();
  hbr_mediatags_add_elems(track);
  nh_map_str_strset_set(hbr_whitelistparents_map, "track", track);

  // <tbody>
  nh_set_str_t tbody = nh_set_str_create();
  nh_set_str_add(tbody, "table");
  nh_map_str_strset_set(hbr_whitelistparents_map, "tbody", tbody);

  // <td>
  nh_set_str_t td = nh_set_str_create();
  nh_set_str_add(td, "tr");
  nh_map_str_strset_set(hbr_whitelistparents_map, "td", td);

  // <tfoot>
  nh_set_str_t tfoot = nh_set_str_create();
  nh_set_str_add(tfoot, "table");
  nh_map_str_strset_set(hbr_whitelistparents_map, "tfoot", tfoot);

  // <th>
  nh_set_str_t th = nh_set_str_create();
  nh_set_str_add(th, "tr");
  nh_map_str_strset_set(hbr_whitelistparents_map, "th", th);

  // <thead>
  nh_set_str_t thead = nh_set_str_create();
  nh_set_str_add(thead, "table");
  nh_map_str_strset_set(hbr_whitelistparents_map, "thead", thead);

  // <tr>
  nh_set_str_t tr = nh_set_str_create();
  nh_set_str_add(tr, "table");
  nh_set_str_add(tr, "thead");
  nh_set_str_add(tr, "tbody");
  nh_set_str_add(tr, "tfoot");
  nh_map_str_strset_set(hbr_whitelistparents_map, "tr", tr);

  // <template>
  // Should be <body>, <frameset>, <head>, <dl>, <colgroup>, but ignoring
}

int hbr_whitelistparents_allowed(hb_char_t *child, hb_char_t *parent) {
  nh_set_str_t set = nh_map_str_strset_get(hbr_whitelistparents_map, (char *) child, NULL);
  return set == NULL || nh_set_str_has(set, (char *) parent);
}

#endif // _HDR_HYPERBUILD_RULE_WHITELISTPARENTS
