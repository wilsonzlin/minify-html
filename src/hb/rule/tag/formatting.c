#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static hb_set_tag_names* hb_rule_tag_formatting_set;

void hb_rule_tag_formatting_add_elems(hb_set_tag_names* set) {
    // Difference to MDN's inline text semantics list: -br, +del, +ins
    hb_set_tag_names_add_whole_literal(set, "a");
    hb_set_tag_names_add_whole_literal(set, "abbr");
    hb_set_tag_names_add_whole_literal(set, "b");
    hb_set_tag_names_add_whole_literal(set, "bdi");
    hb_set_tag_names_add_whole_literal(set, "bdo");
    hb_set_tag_names_add_whole_literal(set, "cite");
    hb_set_tag_names_add_whole_literal(set, "data");
    hb_set_tag_names_add_whole_literal(set, "del");
    hb_set_tag_names_add_whole_literal(set, "dfn");
    hb_set_tag_names_add_whole_literal(set, "em");
    hb_set_tag_names_add_whole_literal(set, "i");
    hb_set_tag_names_add_whole_literal(set, "ins");
    hb_set_tag_names_add_whole_literal(set, "kbd");
    hb_set_tag_names_add_whole_literal(set, "mark");
    hb_set_tag_names_add_whole_literal(set, "q");
    hb_set_tag_names_add_whole_literal(set, "rp");
    hb_set_tag_names_add_whole_literal(set, "rt");
    hb_set_tag_names_add_whole_literal(set, "rtc");
    hb_set_tag_names_add_whole_literal(set, "ruby");
    hb_set_tag_names_add_whole_literal(set, "s");
    hb_set_tag_names_add_whole_literal(set, "samp");
    hb_set_tag_names_add_whole_literal(set, "small");
    hb_set_tag_names_add_whole_literal(set, "span");
    hb_set_tag_names_add_whole_literal(set, "strong");
    hb_set_tag_names_add_whole_literal(set, "sub");
    hb_set_tag_names_add_whole_literal(set, "sup");
    hb_set_tag_names_add_whole_literal(set, "time");
    hb_set_tag_names_add_whole_literal(set, "u");
    hb_set_tag_names_add_whole_literal(set, "var");
    hb_set_tag_names_add_whole_literal(set, "wbr");
}

void hb_rule_tag_formatting_init(void) {
    hb_rule_tag_formatting_set = hb_set_tag_names_create();
    hb_rule_tag_formatting_add_elems(hb_rule_tag_formatting_set);
}

bool hb_rule_tag_formatting_check(nh_view_str* tag) {
    return hb_set_tag_names_has(hb_rule_tag_formatting_set, tag);
}
