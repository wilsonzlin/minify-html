#include <hb/rule.h>
#include <hb/rune.h>

bool hb_rule_tag_valid_check(nh_view_str* tag)
{
	return hb_rule_tag_html_check(tag) || hb_rule_tag_svg_check(tag);
}
