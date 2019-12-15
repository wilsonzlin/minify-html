fn is_valid_tag(tag: &str) -> bool {
	hb_rule_tag_html_check(tag) || hb_rule_tag_svg_check(tag)
}
