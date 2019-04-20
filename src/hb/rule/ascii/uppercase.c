#include <hb/collection.h>
#include <hb/rune.h>
#include <hb/rule.h>

static nh_bitfield_ascii* hb_rule_ascii_uppercase_set;

void hb_rule_ascii_uppercase_add_elems(nh_bitfield_ascii* set) {
    for (char c = 'A'; c <= 'Z'; c++) {
        nh_bitfield_ascii_add(set, c);
    }
}

void hb_rule_ascii_uppercase_init(void) {
    hb_rule_ascii_uppercase_set = nh_bitfield_ascii_create();
    hb_rule_ascii_uppercase_add_elems(hb_rule_ascii_uppercase_set);
}

bool hb_rule_ascii_uppercase_check(hb_rune c) {
    return nh_bitfield_ascii_has(hb_rule_ascii_uppercase_set, c);
}
