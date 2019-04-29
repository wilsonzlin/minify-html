#include <hb/collection.h>
#include <hb/rule.h>
#include <hb/rune.h>

static hb_map_tag_relations* hb_rule_tag_child_blacklist_map;

void hb_rule_tag_child_blacklist_map_add_entries(hb_map_tag_relations* map)
{
	// <address>
	hb_set_tag_names* address = hb_set_tag_names_create();
	hb_rule_tag_heading_add_elems(address);
	hb_rule_tag_sectioning_add_elems(address);
	hb_set_tag_names_add_whole_literal(address, "address");
	hb_set_tag_names_add_whole_literal(address, "header");
	hb_set_tag_names_add_whole_literal(address, "footer");
	hb_map_tag_relations_set_whole_literal(map, "address", address);

	// <audio>
	hb_set_tag_names* audio = hb_set_tag_names_create();
	hb_rule_tag_media_add_elems(audio);
	hb_map_tag_relations_set_whole_literal(map, "audio", audio);

	// <dfn>
	hb_set_tag_names* dfn = hb_set_tag_names_create();
	hb_set_tag_names_add_whole_literal(dfn, "dfn");
	hb_map_tag_relations_set_whole_literal(map, "dfn", dfn);

	// <dt>
	hb_set_tag_names* dt = hb_set_tag_names_create();
	hb_rule_tag_heading_add_elems(dt);
	hb_rule_tag_sectioning_add_elems(dt);
	hb_set_tag_names_add_whole_literal(dt, "header");
	hb_set_tag_names_add_whole_literal(dt, "footer");
	hb_map_tag_relations_set_whole_literal(map, "dt", dt);

	// <footer>
	hb_set_tag_names* footer = hb_set_tag_names_create();
	hb_set_tag_names_add_whole_literal(footer, "header");
	hb_set_tag_names_add_whole_literal(footer, "footer");
	hb_map_tag_relations_set_whole_literal(map, "footer", footer);

	// <form>
	hb_set_tag_names* form = hb_set_tag_names_create();
	hb_set_tag_names_add_whole_literal(form, "form");
	hb_map_tag_relations_set_whole_literal(map, "form", form);

	// <header>
	hb_set_tag_names* header = hb_set_tag_names_create();
	hb_set_tag_names_add_whole_literal(header, "header");
	hb_set_tag_names_add_whole_literal(header, "footer");
	hb_map_tag_relations_set_whole_literal(map, "header", header);

	// <label>
	hb_set_tag_names* label = hb_set_tag_names_create();
	hb_set_tag_names_add_whole_literal(label, "label");
	hb_map_tag_relations_set_whole_literal(map, "label", label);

	// <progress>
	hb_set_tag_names* progress = hb_set_tag_names_create();
	hb_set_tag_names_add_whole_literal(progress, "progress");
	hb_map_tag_relations_set_whole_literal(map, "progress", progress);

	// <th>
	hb_set_tag_names* th = hb_set_tag_names_create();
	hb_rule_tag_heading_add_elems(th);
	hb_rule_tag_sectioning_add_elems(th);
	hb_set_tag_names_add_whole_literal(th, "header");
	hb_set_tag_names_add_whole_literal(th, "footer");
	hb_map_tag_relations_set_whole_literal(map, "th", th);

	// <video>
	hb_set_tag_names* video = hb_set_tag_names_create();
	hb_rule_tag_media_add_elems(video);
	hb_map_tag_relations_set_whole_literal(map, "video", video);
}

void hb_rule_tag_child_blacklist_init(void)
{
	hb_rule_tag_child_blacklist_map = hb_map_tag_relations_create();
	hb_rule_tag_child_blacklist_map_add_entries(
		hb_rule_tag_child_blacklist_map);
}

// Check if a parent is allowed to have a specific child, based on the
// blacklist.
bool hb_rule_tag_child_blacklist_allowed(nh_view_str* parent,
					 nh_view_str* child)
{
	hb_set_tag_names* set = hb_map_tag_relations_get(
		hb_rule_tag_child_blacklist_map, parent);
	return set == NULL || !hb_set_tag_names_has(set, child);
}
