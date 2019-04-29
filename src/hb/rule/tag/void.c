#include <hb/collection.h>
#include <hb/rule.h>
#include <hb/rune.h>

static hb_set_tag_names* hb_rule_tag_void_set;

void hb_rule_tag_void_add_elems(hb_set_tag_names* set)
{
	hb_set_tag_names_add_whole_literal(set, "area");
	hb_set_tag_names_add_whole_literal(set, "base");
	hb_set_tag_names_add_whole_literal(set, "br");
	hb_set_tag_names_add_whole_literal(set, "col");
	hb_set_tag_names_add_whole_literal(set, "embed");
	hb_set_tag_names_add_whole_literal(set, "hr");
	hb_set_tag_names_add_whole_literal(set, "img");
	hb_set_tag_names_add_whole_literal(set, "input");
	hb_set_tag_names_add_whole_literal(set, "keygen");
	hb_set_tag_names_add_whole_literal(set, "link");
	hb_set_tag_names_add_whole_literal(set, "meta");
	hb_set_tag_names_add_whole_literal(set, "param");
	hb_set_tag_names_add_whole_literal(set, "source");
	hb_set_tag_names_add_whole_literal(set, "track");
	hb_set_tag_names_add_whole_literal(set, "wbr");
}

void hb_rule_tag_void_init(void)
{
	hb_rule_tag_void_set = hb_set_tag_names_create();
	hb_rule_tag_void_add_elems(hb_rule_tag_void_set);
}

bool hb_rule_tag_void_check(nh_view_str* tag)
{
	return hb_set_tag_names_has(hb_rule_tag_void_set, tag);
}
