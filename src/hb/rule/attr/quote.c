#include <hb/collection.h>
#include <hb/rule.h>
#include <hb/rune.h>

static nh_bitfield_ascii* hb_rule_attr_quote_set;

void hb_rule_attr_quote_add_elems(nh_bitfield_ascii* set)
{
	// Backtick is not a valid quote character according to
	// https://html.spec.whatwg.org/multipage/introduction.html#intro-early-example
	nh_bitfield_ascii_add(set, '\'');
	nh_bitfield_ascii_add(set, '"');
}

void hb_rule_attr_quote_init(void)
{
	hb_rule_attr_quote_set = nh_bitfield_ascii_create();
	hb_rule_attr_quote_add_elems(hb_rule_attr_quote_set);
}

bool hb_rule_attr_quote_check(hb_rune c)
{
	return nh_bitfield_ascii_has(hb_rule_attr_quote_set, c);
}
