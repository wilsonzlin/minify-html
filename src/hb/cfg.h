#pragma once

#include <stdbool.h>
#include <nicehash/set-str.h>
#include <hb/err.h>
#include <hb/proc.h>

typedef enum {
  HB_CFG_EX_MODE_NONE, // i.e. minify all without exception
  HB_CFG_EX_MODE_CUSTOM,
  HB_CFG_EX_MODE_ALL, // i.e. don't minify ever
} hb_cfg_ex_mode;

typedef struct {
  hb_cfg_ex_mode mode;
  nh_set_str* set;
} hb_cfg_ex;

typedef struct {
	hb_cfg_ex ex_collapse_whitespace;
	hb_cfg_ex ex_destroy_whole_whitespace;
	hb_cfg_ex ex_trim_whitespace;
	hb_err_set suppressed_errors;
	bool trim_class_attributes;
	bool decode_entities;
	bool remove_attr_quotes;
	bool remove_comments;
	bool remove_optional_tags;
	bool remove_tag_whitespace;
} hb_cfg;

bool hb_cfg_should_min(hb_cfg_ex* ex, hb_proc_view view) {
	switch (ex->mode) {
		case HB_CFG_EX_MODE_ALL:
			return false;
		case HB_CFG_EX_MODE_NONE:
			return true;
		default:
			return !nh_set_str_has()
	}
}
