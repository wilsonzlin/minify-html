#include <hb/collection.h>
#include <hb/rule.h>
#include <hb/rune.h>

static nh_bitfield_ascii* hb_rule_ascii_lowercase_set;

void hb_rule_ascii_lowercase_add_elems(nh_bitfield_ascii* set)
{
	for (char c = 'a'; c <= 'z'; c++) {
		nh_bitfield_ascii_add(set, c);
	}
}

void hb_rule_ascii_lowercase_init(void)
{
	hb_rule_ascii_lowercase_set = nh_bitfield_ascii_create();
	hb_rule_ascii_lowercase_add_elems(hb_rule_ascii_lowercase_set);
}

bool hb_rule_ascii_lowercase_check(hb_rune c)
{
	return nh_bitfield_ascii_has(hb_rule_ascii_lowercase_set, c);
}
