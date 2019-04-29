#include <hb/collection.h>
#include <hb/rule.h>
#include <hb/rune.h>

// "wss" stands for whitespace-sensitive.

static hb_set_tag_names* hb_rule_tag_wss_set;

void hb_rule_tag_wss_add_elems(hb_set_tag_names* set)
{
	hb_set_tag_names_add_whole_literal(set, "code");
	hb_set_tag_names_add_whole_literal(set, "pre");
}

void hb_rule_tag_wss_init(void)
{
	hb_rule_tag_wss_set = hb_set_tag_names_create();
	hb_rule_tag_wss_add_elems(hb_rule_tag_wss_set);
}

bool hb_rule_tag_wss_check(nh_view_str* tag)
{
	return hb_set_tag_names_has(hb_rule_tag_wss_set, tag);
}
