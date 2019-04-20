#pragma once

#include <hb/proc.h>

#define HB_UNIT_ENTITY_NONE -1

typedef enum {
    HB_UNIT_ATTR_QUOTED,
    HB_UNIT_ATTR_UNQUOTED,
    HB_UNIT_ATTR_NOVAL,
} hb_unit_attr_type;

hb_unit_attr_type hb_unit_attr_val_quoted(hb_proc* proc, bool should_collapse_and_trim_value_ws);
void hb_unit_attr_val_unquoted(hb_proc* proc);

void hb_unit_content_html(hb_proc* proc, nh_view_str* parent);

nh_view_str hb_unit_tag_name(hb_proc* proc);

int32_t hb_unit_entity(hb_proc* proc);
