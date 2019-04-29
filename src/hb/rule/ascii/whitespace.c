#include <hb/collection.h>
#include <hb/rule.h>
#include <hb/rune.h>

static nh_bitfield_ascii* hb_rule_ascii_whitespace_set;

void hb_rule_ascii_whitespace_add_elems(nh_bitfield_ascii* set)
{
	nh_bitfield_ascii_add(set, '\t'); // TAB
	nh_bitfield_ascii_add(set, '\n'); // LF
	nh_bitfield_ascii_add(set, '\f'); // FF
	nh_bitfield_ascii_add(set, '\r'); // CR
	nh_bitfield_ascii_add(set, ' ');  // SPACE
}

void hb_rule_ascii_whitespace_init(void)
{
	hb_rule_ascii_whitespace_set = nh_bitfield_ascii_create();
	hb_rule_ascii_whitespace_add_elems(hb_rule_ascii_whitespace_set);
}

bool hb_rule_ascii_whitespace_check(hb_rune c)
{
	return nh_bitfield_ascii_has(hb_rule_ascii_whitespace_set, c);
}
