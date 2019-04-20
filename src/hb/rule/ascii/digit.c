#include <hb/collection.h>
#include <hb/rune.h>
#include <hb/rule.h>

static nh_bitfield_ascii* hb_rule_ascii_digit_set;

void hb_rule_ascii_digit_add_elems(nh_bitfield_ascii* set) {
    for (char c = '0'; c <= '9'; c++) {
        nh_bitfield_ascii_add(set, c);
    }
}

void hb_rule_ascii_digit_init(void) {
    hb_rule_ascii_digit_set = nh_bitfield_ascii_create();
    hb_rule_ascii_digit_add_elems(hb_rule_ascii_digit_set);
}

bool hb_rule_ascii_digit_check(hb_rune c) {
    return nh_bitfield_ascii_has(hb_rule_ascii_digit_set, c);
}
