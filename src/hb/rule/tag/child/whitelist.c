#pragma once

#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static nh_map_str_set_str* hb_rule_tag_child_whitelist_map;

void hb_rule_tag_child_whitelist_map_add_entries(nh_map_str_set_str* map) {
    // <colgroup>
    nh_set_str* colgroup = nh_set_str_create();
    nh_set_str_add(colgroup, "col");
    nh_map_str_set_str_set(map, "colgroup", colgroup);

    // <datalist>
    nh_set_str* datalist = nh_set_str_create();
    nh_set_str_add(datalist, "option");
    nh_map_str_set_str_set(map, "datalist", datalist);

    // <dl>
    nh_set_str* dl = nh_set_str_create();
    nh_set_str_add(dl, "dt");
    nh_set_str_add(dl, "dd");
    nh_map_str_set_str_set(map, "dl", dl);

    // <hgroup>
    nh_set_str* hgroup = nh_set_str_create();
    hb_rule_tag_heading_add_elems(hgroup);
    nh_map_str_set_str_set(map, "hgroup", hgroup);

    // <ol>
    nh_set_str* ol = nh_set_str_create();
    nh_set_str_add(ol, "li");
    nh_map_str_set_str_set(map, "ol", ol);

    // <optgroup>
    nh_set_str* optgroup = nh_set_str_create();
    nh_set_str_add(optgroup, "option");
    nh_map_str_set_str_set(map, "optgroup", optgroup);

    // <picture>
    nh_set_str* picture = nh_set_str_create();
    nh_set_str_add(picture, "source");
    nh_set_str_add(picture, "img");
    nh_map_str_set_str_set(map, "picture", picture);

    // <select>
    nh_set_str* select = nh_set_str_create();
    nh_set_str_add(select, "optgroup");
    nh_set_str_add(select, "option");
    nh_map_str_set_str_set(map, "select", select);

    // <table>
    nh_set_str* table = nh_set_str_create();
    nh_set_str_add(table, "caption");
    nh_set_str_add(table, "colgroup");
    nh_set_str_add(table, "col");
    nh_set_str_add(table, "thead");
    nh_set_str_add(table, "tbody");
    nh_set_str_add(table, "tfoot");
    nh_set_str_add(table, "tr");
    nh_map_str_set_str_set(map, "table", table);

    // <tbody>
    nh_set_str* tbody = nh_set_str_create();
    nh_set_str_add(tbody, "tr");
    nh_map_str_set_str_set(map, "tbody", tbody);

    // <tfoot>
    nh_set_str* tfoot = nh_set_str_create();
    nh_set_str_add(tfoot, "tr");
    nh_map_str_set_str_set(map, "tfoot", tfoot);

    // <thead>
    nh_set_str* thead = nh_set_str_create();
    nh_set_str_add(thead, "tr");
    nh_map_str_set_str_set(map, "thead", thead);

    // <tr>
    nh_set_str* tr = nh_set_str_create();
    nh_set_str_add(tr, "td");
    nh_set_str_add(tr, "th");
    nh_set_str_add(tr, "template");
    nh_set_str_add(tr, "script");
    nh_map_str_set_str_set(map, "tr", tr);

    // <ul>
    nh_set_str* ul = nh_set_str_create();
    nh_set_str_add(ul, "li");
    nh_map_str_set_str_set(map, "ul", ul);
}

void hb_rule_tag_child_whitelist_init(void) {
    hb_rule_tag_child_whitelist_map = nh_map_str_set_str_create();
    hb_rule_tag_child_whitelist_map_add_entries(hb_rule_tag_child_whitelist_map);
}

// Check if a parent is allowed to have a specific child.
bool hb_rule_tag_child_whitelist_allowed(hb_rune* parent, hb_rune* child) {
    nh_set_str* set = nh_map_str_set_str_get(hb_rule_tag_child_whitelist_map, parent);
    return set == NULL || nh_set_str_has(set, child);
}
