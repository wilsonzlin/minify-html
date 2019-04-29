#include <hb/collection.h>
#include <hb/rule.h>
#include <hb/rune.h>

static nh_bitfield_ascii* hb_rule_attr_name_exceptions;

void hb_rule_attr_name_add_exceptions(nh_bitfield_ascii* set)
{
	hb_rule_ascii_control_add_elems(set);
	nh_bitfield_ascii_add(set, ' ');
	nh_bitfield_ascii_add(set, '"');
	nh_bitfield_ascii_add(set, '\'');
	nh_bitfield_ascii_add(set, '>');
	nh_bitfield_ascii_add(set, '/');
	nh_bitfield_ascii_add(set, '=');
	// NOTE: Unicode noncharacters not tested
	// (https://html.spec.whatwg.org/multipage/syntax.html#syntax-attribute-name)
}

void hb_rule_attr_name_init(void)
{
	hb_rule_attr_name_exceptions = nh_bitfield_ascii_create();
	hb_rule_attr_name_add_exceptions(hb_rule_attr_name_exceptions);
}

bool hb_rule_attr_name_check(hb_rune c)
{
	return !nh_bitfield_ascii_has(hb_rule_attr_name_exceptions, c);
}
