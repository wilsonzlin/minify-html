#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static hb_set_tag_names* hb_rule_tag_heading_set;

void hb_rule_tag_heading_add_elems(hb_set_tag_names* set) {
    hb_set_tag_names_add_whole_literal(set, "hgroup");
    hb_set_tag_names_add_whole_literal(set, "h1");
    hb_set_tag_names_add_whole_literal(set, "h2");
    hb_set_tag_names_add_whole_literal(set, "h3");
    hb_set_tag_names_add_whole_literal(set, "h4");
    hb_set_tag_names_add_whole_literal(set, "h5");
    hb_set_tag_names_add_whole_literal(set, "h6");
}

void hb_rule_tag_heading_init(void) {
    hb_rule_tag_heading_set = hb_set_tag_names_create();
    hb_rule_tag_heading_add_elems(hb_rule_tag_heading_set);
}

bool hb_rule_tag_heading_check(nh_view_str* tag) {
    return hb_set_tag_names_has(hb_rule_tag_heading_set, tag);
}
