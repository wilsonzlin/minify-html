#include <hb/proc.h>
#include <hb/rule.h>
#include <hb/unit.h>

void hb_unit_tag(hb_proc* proc, nh_view_str* parent)
{
	hb_proc_require(proc, '<');
	nh_view_str name = hb_unit_tag_name(proc);

	// Check that this tag is allowed directly under its parent.
	if (!hb_rule_tag_parent_whitelist_allowed(&name, parent)
	    || !hb_rule_tag_child_whitelist_allowed(parent, &name)
	    || !hb_rule_tag_parent_blacklist_allowed(&name, parent)
	    || !hb_rule_tag_child_blacklist_allowed(parent, &name)) {
		hb_proc_error(proc, HB_ERR_PARSE_ILLEGAL_CHILD,
			      "Tag can't be a child here");
	}

	hb_unit_attr_type last_attr_type = HB_UNIT_ATTR_NONE;
	bool self_closing = false;

	while (true) {
		// At the beginning of this loop, the last parsed unit was
		// either the tag name or an attribute (including its value, if
		// it had one).
		size_t ws_accepted;
		if (proc->cfg->remove_tag_whitespace) {
			ws_accepted = hb_proc_skip_while_predicate(
				proc, &hb_rule_ascii_whitespace_check);
		} else {
			ws_accepted = hb_proc_accept_while_predicate(
				proc, &hb_rule_ascii_whitespace_check);
		}

		if (hb_proc_accept_if(proc, '>')) {
			// End of tag.
			break;
		}

		if ((self_closing = hb_proc_accept_if_matches(proc, "/>"))) {
			hb_proc_error_if_not_suppressed(
				proc, HB_ERR_PARSE_SELF_CLOSING_TAG,
				"Self-closing tag");
			break;
		}

		// HB_ERR_PARSE_NO_SPACE_BEFORE_ATTR is not suppressible as
		// otherwise there would be difficulty in determining what is
		// the end of a tag/attribute name/attribute value.
		if (!ws_accepted) {
			hb_proc_error(proc, HB_ERR_PARSE_NO_SPACE_BEFORE_ATTR,
				      "No whitespace before attribute");
		}

		if (proc->cfg->remove_tag_whitespace) {
			if (last_attr_type != HB_UNIT_ATTR_QUOTED) {
				hb_proc_write(proc, ' ');
			}
		}

		last_attr_type = hb_unit_attr(proc);
	}

	if (self_closing || hb_rule_tag_void_check(&name)) {
		return;
	}

	if (nh_view_str_equals_literal_i(&name, "script")) {
		// <script> tag.
		hb_unit_content_script(proc);
	} else if (nh_view_str_equals_literal_i(&name, "style")) {
		// <style> tag.
		hb_unit_content_style(proc);
	} else {
		// Standard HTML.
		hb_unit_content_html(proc, &name);
	}

	// Require closing tag for non-void.
	hb_proc_require_match(proc, "</");
	nh_view_str closing_name = hb_unit_tag_name(proc);
	if (!nh_view_str_equals(&name, &closing_name)) {
		// TODO Find a way to cleanly provide opening and closing tag
		// names (which are views) into error message without leaking
		// memory.
		hb_proc_error(proc, HB_ERR_PARSE_UNCLOSED_TAG,
			      "Tag not closed");
	}
	hb_proc_require(proc, '>');
}
