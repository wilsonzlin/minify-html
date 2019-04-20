#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static hb_map_tag_relations* hb_rule_tag_child_whitelist_map;

void hb_rule_tag_child_whitelist_map_add_entries(hb_map_tag_relations* map) {
    // <colgroup>
    hb_set_tag_names* colgroup = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(colgroup, "col");
    hb_map_tag_relations_set_whole_literal(map, "colgroup", colgroup);

    // <datalist>
    hb_set_tag_names* datalist = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(datalist, "option");
    hb_map_tag_relations_set_whole_literal(map, "datalist", datalist);

    // <dl>
    hb_set_tag_names* dl = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(dl, "dt");
    hb_set_tag_names_add_whole_literal(dl, "dd");
    hb_map_tag_relations_set_whole_literal(map, "dl", dl);

    // <hgroup>
    hb_set_tag_names* hgroup = hb_set_tag_names_create();
    hb_rule_tag_heading_add_elems(hgroup);
    hb_map_tag_relations_set_whole_literal(map, "hgroup", hgroup);

    // <ol>
    hb_set_tag_names* ol = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(ol, "li");
    hb_map_tag_relations_set_whole_literal(map, "ol", ol);

    // <optgroup>
    hb_set_tag_names* optgroup = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(optgroup, "option");
    hb_map_tag_relations_set_whole_literal(map, "optgroup", optgroup);

    // <picture>
    hb_set_tag_names* picture = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(picture, "source");
    hb_set_tag_names_add_whole_literal(picture, "img");
    hb_map_tag_relations_set_whole_literal(map, "picture", picture);

    // <select>
    hb_set_tag_names* select = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(select, "optgroup");
    hb_set_tag_names_add_whole_literal(select, "option");
    hb_map_tag_relations_set_whole_literal(map, "select", select);

    // <table>
    hb_set_tag_names* table = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(table, "caption");
    hb_set_tag_names_add_whole_literal(table, "colgroup");
    hb_set_tag_names_add_whole_literal(table, "col");
    hb_set_tag_names_add_whole_literal(table, "thead");
    hb_set_tag_names_add_whole_literal(table, "tbody");
    hb_set_tag_names_add_whole_literal(table, "tfoot");
    hb_set_tag_names_add_whole_literal(table, "tr");
    hb_map_tag_relations_set_whole_literal(map, "table", table);

    // <tbody>
    hb_set_tag_names* tbody = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(tbody, "tr");
    hb_map_tag_relations_set_whole_literal(map, "tbody", tbody);

    // <tfoot>
    hb_set_tag_names* tfoot = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(tfoot, "tr");
    hb_map_tag_relations_set_whole_literal(map, "tfoot", tfoot);

    // <thead>
    hb_set_tag_names* thead = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(thead, "tr");
    hb_map_tag_relations_set_whole_literal(map, "thead", thead);

    // <tr>
    hb_set_tag_names* tr = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(tr, "td");
    hb_set_tag_names_add_whole_literal(tr, "th");
    hb_set_tag_names_add_whole_literal(tr, "template");
    hb_set_tag_names_add_whole_literal(tr, "script");
    hb_map_tag_relations_set_whole_literal(map, "tr", tr);

    // <ul>
    hb_set_tag_names* ul = hb_set_tag_names_create();
    hb_set_tag_names_add_whole_literal(ul, "li");
    hb_map_tag_relations_set_whole_literal(map, "ul", ul);
}

void hb_rule_tag_child_whitelist_init(void) {
    hb_rule_tag_child_whitelist_map = hb_map_tag_relations_create();
    hb_rule_tag_child_whitelist_map_add_entries(hb_rule_tag_child_whitelist_map);
}

// Check if a parent is allowed to have a specific child.
bool hb_rule_tag_child_whitelist_allowed(nh_view_str* parent, nh_view_str* child) {
    hb_set_tag_names* set = hb_map_tag_relations_get(hb_rule_tag_child_whitelist_map, parent);
    return set == NULL || hb_set_tag_names_has(set, child);
}
