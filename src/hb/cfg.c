#include <hb/cfg.h>

bool hb_cfg_should_min(hb_cfg_tags_set* set, nh_view_str* view) {
    switch (set->mode) {
        case HB_CFG_SET_MODE_NONE:
            return false;
        case HB_CFG_SET_MODE_ALL:
            return true;
        case HB_CFG_SET_MODE_ALLOW:
            return hb_set_tag_names_has(set->set, view);
        default: /* case HB_CFG_SET_MODE_DENY: */
            return !hb_set_tag_names_has(set->set, view);
    }
}
