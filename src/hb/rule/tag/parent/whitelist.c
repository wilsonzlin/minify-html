#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static hb_map_tag_relations* hb_rule_tag_parent_whitelist_map;

void hb_rule_tag_parent_whitelist_map_add_entries(hb_map_tag_relations* map) {
    // <caption>
    hb_set_tag_names* caption = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(caption, "table");
    hb_map_tag_relations_set_whole_literal(map, "caption", caption);

    // <col>
    hb_set_tag_names* col = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(col, "table");
    hb_set_tag_names_add_whole_literal(col, "colgroup");
    hb_map_tag_relations_set_whole_literal(map, "col", col);

    // <colgroup>
    hb_set_tag_names* colgroup = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(colgroup, "table");
    hb_map_tag_relations_set_whole_literal(map, "colgroup", colgroup);

    // <dd>
    hb_set_tag_names* dd = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(dd, "dl");
    hb_map_tag_relations_set_whole_literal(map, "dd", dd);

    // <dt>
    hb_set_tag_names* dt = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(dt, "dl");
    hb_map_tag_relations_set_whole_literal(map, "dt", dt);

    // <figcaption>
    hb_set_tag_names* figcaption = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(figcaption, "figure");
    hb_map_tag_relations_set_whole_literal(map, "figcaption", figcaption);

    // <legend>
    hb_set_tag_names* legend = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(legend, "fieldset");
    hb_map_tag_relations_set_whole_literal(map, "legend", legend);

    // <li>
    hb_set_tag_names* li = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(li, "ul");
    hb_set_tag_names_add_whole_literal(li, "ol");
    hb_set_tag_names_add_whole_literal(li, "menu");
    hb_map_tag_relations_set_whole_literal(map, "li", li);

    // <optgroup>
    hb_set_tag_names* optgroup = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(optgroup, "select");
    hb_map_tag_relations_set_whole_literal(map, "optgroup", optgroup);

    // <option>
    hb_set_tag_names* option = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(option, "select");
    hb_set_tag_names_add_whole_literal(option, "optgroup");
    hb_set_tag_names_add_whole_literal(option, "datalist");
    hb_map_tag_relations_set_whole_literal(map, "option", option);

    // <param>
    hb_set_tag_names* param = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(param, "object");
    hb_map_tag_relations_set_whole_literal(map, "param", param);

    // <rp>
    hb_set_tag_names* rp = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(rp, "ruby");
    hb_map_tag_relations_set_whole_literal(map, "rp", rp);

    // <rt>
    hb_set_tag_names* rt = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(rt, "ruby");
    hb_map_tag_relations_set_whole_literal(map, "rt", rt);

    // <rtc>
    hb_set_tag_names* rtc = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(rtc, "ruby");
    hb_map_tag_relations_set_whole_literal(map, "rtc", rtc);

    // <summary>
    hb_set_tag_names* summary = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(summary, "details");
    hb_map_tag_relations_set_whole_literal(map, "summary", summary);

    // <source>
    hb_set_tag_names* source = hb_set_tag_names_create();
    hb_rule_tag_media_add_elems(source);
    hb_set_tag_names_add_whole_literal(source, "picture");
    hb_map_tag_relations_set_whole_literal(map, "source", source);

    // <track>
    hb_set_tag_names* track = hb_set_tag_names_create();
    hb_rule_tag_media_add_elems(track);
    hb_map_tag_relations_set_whole_literal(map, "track", track);

    // <tbody>
    hb_set_tag_names* tbody = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(tbody, "table");
    hb_map_tag_relations_set_whole_literal(map, "tbody", tbody);

    // <td>
    hb_set_tag_names* td = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(td, "tr");
    hb_map_tag_relations_set_whole_literal(map, "td", td);

    // <tfoot>
    hb_set_tag_names* tfoot = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(tfoot, "table");
    hb_map_tag_relations_set_whole_literal(map, "tfoot", tfoot);

    // <th>
    hb_set_tag_names* th = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(th, "tr");
    hb_map_tag_relations_set_whole_literal(map, "th", th);

    // <thead>
    hb_set_tag_names* thead = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(thead, "table");
    hb_map_tag_relations_set_whole_literal(map, "thead", thead);

    // <tr>
    hb_set_tag_names* tr = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(tr, "table");
    hb_set_tag_names_add_whole_literal(tr, "thead");
    hb_set_tag_names_add_whole_literal(tr, "tbody");
    hb_set_tag_names_add_whole_literal(tr, "tfoot");
    hb_map_tag_relations_set_whole_literal(map, "tr", tr);

    // <template>
    // Should be <body>, <frameset>, <head>, <dl>, <colgroup>, but ignoring.
}

void hb_rule_tag_parent_whitelist_init(void) {
    hb_rule_tag_parent_whitelist_map = hb_map_tag_relations_create();
    hb_rule_tag_parent_whitelist_map_add_entries(hb_rule_tag_parent_whitelist_map);
}

// Check if a child is allowed to have a specific parent.
bool hb_rule_tag_parent_whitelist_allowed(nh_view_str* child, nh_view_str* parent) {
    hb_set_tag_names* set = hb_map_tag_relations_get(hb_rule_tag_parent_whitelist_map, child);
    return set == NULL || hb_set_tag_names_has(set, parent);
}
