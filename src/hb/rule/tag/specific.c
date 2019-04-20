#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static hb_set_tag_names* hb_rule_tag_specific_set;

void hb_rule_tag_specific_add_elems(hb_set_tag_names* set) {
    hb_rule_tag_svg_add_elems(set);
    hb_set_tag_names_add_whole_literal(set, "area");
    hb_set_tag_names_add_whole_literal(set, "base");
    hb_set_tag_names_add_whole_literal(set, "br");
    hb_set_tag_names_add_whole_literal(set, "code"); // Reason: unlikely to want to minify
    hb_set_tag_names_add_whole_literal(set, "col");
    hb_set_tag_names_add_whole_literal(set, "embed");
    hb_set_tag_names_add_whole_literal(set, "hr");
    hb_set_tag_names_add_whole_literal(set, "img");
    hb_set_tag_names_add_whole_literal(set, "input");
    hb_set_tag_names_add_whole_literal(set, "param");
    hb_set_tag_names_add_whole_literal(set, "pre"); // Reason: unlikely to want to minify
    hb_set_tag_names_add_whole_literal(set, "script");
    hb_set_tag_names_add_whole_literal(set, "source");
    hb_set_tag_names_add_whole_literal(set, "track");
}

void hb_rule_tag_specific_init(void) {
    hb_rule_tag_specific_set = hb_set_tag_names_create();
    hb_rule_tag_specific_add_elems(hb_rule_tag_specific_set);
}

bool hb_rule_tag_specific_check(nh_view_str* tag) {
    return hb_set_tag_names_has(hb_rule_tag_specific_set, tag);
}
