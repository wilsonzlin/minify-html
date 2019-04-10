#pragma once

#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static nh_map_str_set_str* hb_rule_tag_child_blacklist_map;

void hb_rule_tag_child_blacklist_map_add_entries(nh_map_str_set_str* map) {
    // <address>
    nh_set_str* address = nh_set_str_create();
    hb_rule_tag_heading_add_elems(address);
    hb_rule_tag_sectioning_add_elems(address);
    nh_set_str_add(address, "address");
    nh_set_str_add(address, "header");
    nh_set_str_add(address, "footer");
    nh_map_str_set_str_set(map, "address", address);

    // <audio>
    nh_set_str* audio = nh_set_str_create();
    hb_rule_tag_media_add_elems(audio);
    nh_map_str_set_str_set(map, "audio", audio);

    // <dfn>
    nh_set_str* dfn = nh_set_str_create();
    nh_set_str_add(dfn, "dfn");
    nh_map_str_set_str_set(map, "dfn", dfn);

    // <dt>
    nh_set_str* dt = nh_set_str_create();
    hb_rule_tag_heading_add_elems(dt);
    hb_rule_tag_sectioning_add_elems(dt);
    nh_set_str_add(dt, "header");
    nh_set_str_add(dt, "footer");
    nh_map_str_set_str_set(map, "dt", dt);

    // <footer>
    nh_set_str* footer = nh_set_str_create();
    nh_set_str_add(footer, "header");
    nh_set_str_add(footer, "footer");
    nh_map_str_set_str_set(map, "footer", footer);

    // <form>
    nh_set_str* form = nh_set_str_create();
    nh_set_str_add(form, "form");
    nh_map_str_set_str_set(map, "form", form);

    // <header>
    nh_set_str* header = nh_set_str_create();
    nh_set_str_add(header, "header");
    nh_set_str_add(header, "footer");
    nh_map_str_set_str_set(map, "header", header);

    // <label>
    nh_set_str* label = nh_set_str_create();
    nh_set_str_add(label, "label");
    nh_map_str_set_str_set(map, "label", label);

    // <progress>
    nh_set_str* progress = nh_set_str_create();
    nh_set_str_add(progress, "progress");
    nh_map_str_set_str_set(map, "progress", progress);

    // <th>
    nh_set_str* th = nh_set_str_create();
    hb_rule_tag_heading_add_elems(th);
    hb_rule_tag_sectioning_add_elems(th);
    nh_set_str_add(th, "header");
    nh_set_str_add(th, "footer");
    nh_map_str_set_str_set(map, "th", th);

    // <video>
    nh_set_str* video = nh_set_str_create();
    hb_rule_tag_media_add_elems(video);
    nh_map_str_set_str_set(map, "video", video);
}

void hb_rule_tag_child_blacklist_init(void) {
    hb_rule_tag_child_blacklist_map = nh_map_str_set_str_create();
    hb_rule_tag_child_blacklist_map_add_entries(hb_rule_tag_child_blacklist_map);
}

// Check if a parent is allowed to have a specific child, based on the blacklist.
bool hb_rule_tag_child_blacklist_allowed(hb_rune* parent, hb_rune* child) {
    nh_set_str* set = nh_map_str_set_str_get(hb_rule_tag_child_blacklist_map, parent);
    return set == NULL || !nh_set_str_has(set, child);
}
