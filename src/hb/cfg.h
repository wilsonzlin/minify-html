#pragma once

#include <hb/collection.h>
#include <hb/err.h>
#include <stdbool.h>

typedef enum {
	HB_CFG_SET_MODE_NONE, // i.e. don't minify ever
	HB_CFG_SET_MODE_ALLOW,
	HB_CFG_SET_MODE_DENY,
	HB_CFG_SET_MODE_ALL, // i.e. minify all without exception
} hb_cfg_tags_set_mode;

typedef struct {
	hb_cfg_tags_set_mode mode;
	hb_set_tag_names* set;
} hb_cfg_tags_set;

typedef struct {
	hb_cfg_tags_set collapse_whitespace;
	hb_cfg_tags_set destroy_whole_whitespace;
	hb_cfg_tags_set trim_whitespace;
	hb_err_set suppressed_errors;
	bool trim_class_attributes;
	bool decode_entities;
	bool remove_attr_quotes;
	bool remove_comments;
	bool remove_optional_tags;
	bool remove_tag_whitespace;
} hb_cfg;

bool hb_cfg_should_min(hb_cfg_tags_set* set, nh_view_str* view);
