#include <hb/collection.h>
#include <hb/rule.h>
#include <hb/rune.h>

static hb_set_tag_names* hb_rule_tag_layout_set;

void hb_rule_tag_layout_add_elems(hb_set_tag_names* set)
{
	hb_rule_tag_sectioning_add_elems(set);
	hb_set_tag_names_add_whole_literal(set, "blockquote");
	hb_set_tag_names_add_whole_literal(set, "body");
	hb_set_tag_names_add_whole_literal(set, "colgroup");
	hb_set_tag_names_add_whole_literal(set, "datalist");
	hb_set_tag_names_add_whole_literal(set, "dialog");
	hb_set_tag_names_add_whole_literal(set, "div");
	hb_set_tag_names_add_whole_literal(set, "dl");
	hb_set_tag_names_add_whole_literal(set, "fieldset");
	hb_set_tag_names_add_whole_literal(set, "figure");
	hb_set_tag_names_add_whole_literal(set, "footer");
	hb_set_tag_names_add_whole_literal(set, "form");
	hb_set_tag_names_add_whole_literal(set, "head");
	hb_set_tag_names_add_whole_literal(set, "header");
	hb_set_tag_names_add_whole_literal(set, "hgroup");
	hb_set_tag_names_add_whole_literal(set, "html");
	hb_set_tag_names_add_whole_literal(set, "main");
	hb_set_tag_names_add_whole_literal(set, "map");
	hb_set_tag_names_add_whole_literal(set, "menu");
	hb_set_tag_names_add_whole_literal(set, "nav");
	hb_set_tag_names_add_whole_literal(set, "ol");
	hb_set_tag_names_add_whole_literal(set, "optgroup");
	hb_set_tag_names_add_whole_literal(set, "picture");
	hb_set_tag_names_add_whole_literal(set, "section");
	hb_set_tag_names_add_whole_literal(set, "select");
	hb_set_tag_names_add_whole_literal(set, "table");
	hb_set_tag_names_add_whole_literal(set, "tbody");
	hb_set_tag_names_add_whole_literal(set, "tfoot");
	hb_set_tag_names_add_whole_literal(set, "thead");
	hb_set_tag_names_add_whole_literal(set, "tr");
	hb_set_tag_names_add_whole_literal(set, "ul");
}

void hb_rule_tag_layout_init(void)
{
	hb_rule_tag_layout_set = hb_set_tag_names_create();
	hb_rule_tag_layout_add_elems(hb_rule_tag_layout_set);
}

bool hb_rule_tag_layout_check(nh_view_str* tag)
{
	return hb_set_tag_names_has(hb_rule_tag_layout_set, tag);
}
