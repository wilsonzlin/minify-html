#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static hb_set_tag_names* hb_rule_tag_content_set;

void hb_rule_tag_content_add_elems(hb_set_tag_names* set) {
    hb_set_tag_names_add_whole_literal(set, "address");
    hb_set_tag_names_add_whole_literal(set, "audio");
    hb_set_tag_names_add_whole_literal(set, "button");
    hb_set_tag_names_add_whole_literal(set, "canvas");
    hb_set_tag_names_add_whole_literal(set, "caption");
    hb_set_tag_names_add_whole_literal(set, "figcaption");
    hb_set_tag_names_add_whole_literal(set, "h1");
    hb_set_tag_names_add_whole_literal(set, "h2");
    hb_set_tag_names_add_whole_literal(set, "h3");
    hb_set_tag_names_add_whole_literal(set, "h4");
    hb_set_tag_names_add_whole_literal(set, "h5");
    hb_set_tag_names_add_whole_literal(set, "h6");
    hb_set_tag_names_add_whole_literal(set, "legend");
    hb_set_tag_names_add_whole_literal(set, "meter");
    hb_set_tag_names_add_whole_literal(set, "object");
    hb_set_tag_names_add_whole_literal(set, "option");
    hb_set_tag_names_add_whole_literal(set, "p");
    hb_set_tag_names_add_whole_literal(set, "summary"); // Can also contain a heading
    hb_set_tag_names_add_whole_literal(set, "textarea");
    hb_set_tag_names_add_whole_literal(set, "video");
}

void hb_rule_tag_content_init(void) {
    hb_rule_tag_content_set = hb_set_tag_names_create();
    hb_rule_tag_content_add_elems(hb_rule_tag_content_set);
}

bool hb_rule_tag_content_check(nh_view_str* tag) {
    return hb_set_tag_names_has(hb_rule_tag_content_set, tag);
}
