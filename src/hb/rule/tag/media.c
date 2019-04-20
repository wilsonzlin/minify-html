#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

static hb_set_tag_names* hb_rule_tag_media_set;

void hb_rule_tag_media_add_elems(hb_set_tag_names* set) {
    hb_set_tag_names_add_whole_literal(set, "audio");
    hb_set_tag_names_add_whole_literal(set, "video");
}

void hb_rule_tag_media_init(void) {
    hb_rule_tag_media_set = hb_set_tag_names_create();
    hb_rule_tag_media_add_elems(hb_rule_tag_media_set);
}

bool hb_rule_tag_media_check(nh_view_str* tag) {
    return hb_set_tag_names_has(hb_rule_tag_media_set, tag);
}
