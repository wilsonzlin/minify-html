#include <hb/collection.h>
#include <hb/rune.h>
#include <hb/rule.h>

static nh_bitfield_ascii* hb_rule_tag_name_set;

void hb_rule_tag_name_add_elems(nh_bitfield_ascii* set) {
    hb_rule_ascii_lowercase_add_elems(set);
    hb_rule_ascii_uppercase_add_elems(set);
    hb_rule_ascii_digit_add_elems(set);
    nh_bitfield_ascii_add(set, ':');
    nh_bitfield_ascii_add(set, '-');
}

void hb_rule_tag_name_init(void) {
    hb_rule_tag_name_set = nh_bitfield_ascii_create();
    hb_rule_tag_name_add_elems(hb_rule_tag_name_set);
}

bool hb_rule_tag_name_check(hb_rune c) {
    return nh_bitfield_ascii_has(hb_rule_tag_name_set, c);
}
