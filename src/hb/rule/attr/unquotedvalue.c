#include <hb/collection.h>
#include <hb/rule.h>
#include <hb/rune.h>

static nh_bitfield_ascii* hb_rule_attr_unquotedvalue_exceptions;

void hb_rule_attr_unquotedvalue_add_exceptions(nh_bitfield_ascii* set)
{
	hb_rule_ascii_whitespace_add_elems(set);
	nh_bitfield_ascii_add(set, '"');
	nh_bitfield_ascii_add(set, '\'');
	nh_bitfield_ascii_add(set, '`');
	nh_bitfield_ascii_add(set, '=');
	nh_bitfield_ascii_add(set, '<');
	nh_bitfield_ascii_add(set, '>');
}

void hb_rule_attr_unquotedvalue_init(void)
{
	hb_rule_attr_unquotedvalue_exceptions = nh_bitfield_ascii_create();
	hb_rule_attr_unquotedvalue_add_exceptions(
		hb_rule_attr_unquotedvalue_exceptions);
}

bool hb_rule_attr_unquotedvalue_check(hb_rune c)
{
	return !nh_bitfield_ascii_has(hb_rule_attr_unquotedvalue_exceptions, c);
}
