#pragma once

#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static nh_map_str_set_str* hb_rule_tag_parent_whitelist_map;

void hb_rule_tag_parent_whitelist_map_add_entries(nh_map_str_set_str* map) {
    // <caption>
    nh_set_str* caption = nh_set_str_create();
    nh_set_str_add(caption, "table");
    nh_map_str_set_str_set(map, "caption", caption);

    // <col>
    nh_set_str* col = nh_set_str_create();
    nh_set_str_add(col, "table");
    nh_set_str_add(col, "colgroup");
    nh_map_str_set_str_set(map, "col", col);

    // <colgroup>
    nh_set_str* colgroup = nh_set_str_create();
    nh_set_str_add(colgroup, "table");
    nh_map_str_set_str_set(map, "colgroup", colgroup);

    // <dd>
    nh_set_str* dd = nh_set_str_create();
    nh_set_str_add(dd, "dl");
    nh_map_str_set_str_set(map, "dd", dd);

    // <dt>
    nh_set_str* dt = nh_set_str_create();
    nh_set_str_add(dt, "dl");
    nh_map_str_set_str_set(map, "dt", dt);

    // <figcaption>
    nh_set_str* figcaption = nh_set_str_create();
    nh_set_str_add(figcaption, "figure");
    nh_map_str_set_str_set(map, "figcaption", figcaption);

    // <legend>
    nh_set_str* legend = nh_set_str_create();
    nh_set_str_add(legend, "fieldset");
    nh_map_str_set_str_set(map, "legend", legend);

    // <li>
    nh_set_str* li = nh_set_str_create();
    nh_set_str_add(li, "ul");
    nh_set_str_add(li, "ol");
    nh_set_str_add(li, "menu");
    nh_map_str_set_str_set(map, "li", li);

    // <optgroup>
    nh_set_str* optgroup = nh_set_str_create();
    nh_set_str_add(optgroup, "select");
    nh_map_str_set_str_set(map, "optgroup", optgroup);

    // <option>
    nh_set_str* option = nh_set_str_create();
    nh_set_str_add(option, "select");
    nh_set_str_add(option, "optgroup");
    nh_set_str_add(option, "datalist");
    nh_map_str_set_str_set(map, "option", option);

    // <param>
    nh_set_str* param = nh_set_str_create();
    nh_set_str_add(param, "object");
    nh_map_str_set_str_set(map, "param", param);

    // <rp>
    nh_set_str* rp = nh_set_str_create();
    nh_set_str_add(rp, "ruby");
    nh_map_str_set_str_set(map, "rp", rp);

    // <rt>
    nh_set_str* rt = nh_set_str_create();
    nh_set_str_add(rt, "ruby");
    nh_map_str_set_str_set(map, "rt", rt);

    // <rtc>
    nh_set_str* rtc = nh_set_str_create();
    nh_set_str_add(rtc, "ruby");
    nh_map_str_set_str_set(map, "rtc", rtc);

    // <summary>
    nh_set_str* summary = nh_set_str_create();
    nh_set_str_add(summary, "details");
    nh_map_str_set_str_set(map, "summary", summary);

    // <source>
    nh_set_str* source = nh_set_str_create();
    hb_rule_tag_media_add_elems(source);
    nh_set_str_add(source, "picture");
    nh_map_str_set_str_set(map, "source", source);

    // <track>
    nh_set_str* track = nh_set_str_create();
    hb_rule_tag_media_add_elems(track);
    nh_map_str_set_str_set(map, "track", track);

    // <tbody>
    nh_set_str* tbody = nh_set_str_create();
    nh_set_str_add(tbody, "table");
    nh_map_str_set_str_set(map, "tbody", tbody);

    // <td>
    nh_set_str* td = nh_set_str_create();
    nh_set_str_add(td, "tr");
    nh_map_str_set_str_set(map, "td", td);

    // <tfoot>
    nh_set_str* tfoot = nh_set_str_create();
    nh_set_str_add(tfoot, "table");
    nh_map_str_set_str_set(map, "tfoot", tfoot);

    // <th>
    nh_set_str* th = nh_set_str_create();
    nh_set_str_add(th, "tr");
    nh_map_str_set_str_set(map, "th", th);

    // <thead>
    nh_set_str* thead = nh_set_str_create();
    nh_set_str_add(thead, "table");
    nh_map_str_set_str_set(map, "thead", thead);

    // <tr>
    nh_set_str* tr = nh_set_str_create();
    nh_set_str_add(tr, "table");
    nh_set_str_add(tr, "thead");
    nh_set_str_add(tr, "tbody");
    nh_set_str_add(tr, "tfoot");
    nh_map_str_set_str_set(map, "tr", tr);

    // <template>
    // Should be <body>, <frameset>, <head>, <dl>, <colgroup>, but ignoring
}

void hb_rule_tag_parent_whitelist_init(void) {
    hb_rule_tag_parent_whitelist_map = nh_map_str_set_str_create();
    hb_rule_tag_parent_whitelist_map_add_entries(hb_rule_tag_parent_whitelist_map);
}

// Check if a child is allowed to have a specific parent.
bool hb_rule_tag_parent_whitelist_allowed(hb_rune* child, hb_rune* parent) {
    nh_set_str* set = nh_map_str_set_str_get(hb_rule_tag_parent_whitelist_map, child);
    return set == NULL || nh_set_str_has(set, parent);
}
