#include <hb/collection.h>
#include <hb/rule.h>
#include <hb/rune.h>

static hb_set_tag_names* hb_rule_tag_contentfirst_set;

void hb_rule_tag_contentfirst_add_elems(hb_set_tag_names* set)
{
	hb_set_tag_names_add_whole_literal(set, "dd");
	hb_set_tag_names_add_whole_literal(set, "details");
	hb_set_tag_names_add_whole_literal(set, "dt");
	hb_set_tag_names_add_whole_literal(set, "iframe");
	hb_set_tag_names_add_whole_literal(set, "label");
	hb_set_tag_names_add_whole_literal(set, "li");
	hb_set_tag_names_add_whole_literal(set, "noscript");
	hb_set_tag_names_add_whole_literal(set, "output");
	hb_set_tag_names_add_whole_literal(set, "progress");
	hb_set_tag_names_add_whole_literal(set, "slot");
	hb_set_tag_names_add_whole_literal(set, "td");
	hb_set_tag_names_add_whole_literal(set, "template");
	hb_set_tag_names_add_whole_literal(set, "th");
}

void hb_rule_tag_contentfirst_init(void)
{
	hb_rule_tag_contentfirst_set = hb_set_tag_names_create();
	hb_rule_tag_contentfirst_add_elems(hb_rule_tag_contentfirst_set);
}

bool hb_rule_tag_contentfirst_check(nh_view_str* tag)
{
	return hb_set_tag_names_has(hb_rule_tag_contentfirst_set, tag);
}
