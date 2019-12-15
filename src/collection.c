#include <hb/collection.h>

// Data structure for mapping entity references to Unicode code points.
NH_MAP_VIEW_STR_IMPL(hb_map_entity_references, int32_t, -1);

// Data structure for a set of tag names.
NH_SET_VIEW_ISTR_IMPL(hb_set_tag_names);
#define hb_set_tag_names_add_whole_literal(set, str)                           \
	hb_set_tag_names_add_whole_array(set, nh_litarr(str))

// Data structure for mapping tag names to sets of tag names.
NH_MAP_VIEW_ISTR_IMPL(hb_map_tag_relations, hb_set_tag_names*, NULL);
#define hb_map_tag_relations_set_whole_literal(map, str, v)                    \
	hb_map_tag_relations_set_whole_array(map, nh_litarr(str), v)
