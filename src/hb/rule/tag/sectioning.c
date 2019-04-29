#include <hb/collection.h>
#include <hb/rule.h>
#include <hb/rune.h>

static hb_set_tag_names* hb_rule_tag_sectioning_set;

void hb_rule_tag_sectioning_add_elems(hb_set_tag_names* set)
{
	hb_set_tag_names_add_whole_literal(set, "article");
	hb_set_tag_names_add_whole_literal(set, "aside");
	hb_set_tag_names_add_whole_literal(set, "nav");
	hb_set_tag_names_add_whole_literal(set, "section");
}

void hb_rule_tag_sectioning_init(void)
{
	hb_rule_tag_sectioning_set = hb_set_tag_names_create();
	hb_rule_tag_sectioning_add_elems(hb_rule_tag_sectioning_set);
}

bool hb_rule_tag_sectioning_check(nh_view_str* tag)
{
	return hb_set_tag_names_has(hb_rule_tag_sectioning_set, tag);
}
