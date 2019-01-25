#pragma once

#include <stddef.h>
#include <stdbool.h>

#include "hb-rule.h"
#include "hb-config.h"

static struct hb_config_ex_s _ex_collapse_whitespace_default;
static struct hb_config_ex_s _ex_destroy_whole_whitespace_default;
static struct hb_config_ex_s _ex_trim_whitespace_default;

// WARNING: Rules must be initialised before calling this function
void hb_config_init(void)
{
	nh_set_str ex_collapse_whitespace_set = nh_set_str_create();
	hb_rule_wsstags_add_elems(ex_collapse_whitespace_set);
	_ex_collapse_whitespace_default = {HB_CONFIG_EX_MODE_DEFAULT,
					   ex_collapse_whitespace_set};

	nh_set_str ex_destroy_whole_whitespace_set = nh_set_str_create();
	hb_rule_wsstags_add_elems(ex_destroy_whole_whitespace_set);
	hb_rule_contenttags_add_elems(ex_destroy_whole_whitespace_set);
	hb_rule_formattingtags_add_elems(ex_destroy_whole_whitespace_set);
	_ex_destroy_whole_whitespace_default = {
		HB_CONFIG_EX_MODE_DEFAULT, ex_destroy_whole_whitespace_set};

	nh_set_str ex_trim_whitespace_set = nh_set_str_create();
	hb_rule_wsstags_add_elems(ex_trim_whitespace_set);
	hb_rule_formattingtags_add_elems(ex_trim_whitespace_set);
	_ex_trim_whitespace_default = {HB_CONFIG_EX_MODE_DEFAULT,
				       ex_trim_whitespace_set};
}

hb_config_t* hb_config_create(void)
{
	hb_config_t* config = malloc(sizeof(struct hb_config_s));
	config->ex_collapse_whitespace = _ex_collapse_whitespace_default;
	config->ex_destroy_whole_whitespace =
		_ex_destroy_whole_whitespace_default;
	config->ex_trim_whitespace = _ex_trim_whitespace_default;
	config->suppressed_errors = nh_set_int32_create();
	config->trim_class_attr = true;
	config->decode_entities = true;
	config->min_conditional_comments = true;
	config->remove_attr_quotes = true;
	config->remove_comments = true;
	config->remove_optional_tags = true;
	config->remove_tag_whitespace = true;
	return config;
}

void hb_config_ex_use_none(hb_config_ex_t* config_ex)
{
	*config_ex = {HB_CONFIG_EX_MODE_NONE, NULL};
}

void hb_config_ex_use_custom(hb_config_ex_t* config_ex, nh_set_str custom_set)
{
	*config_ex = {HB_CONFIG_EX_MODE_CUSTOM, custom_set};
}

void hb_config_ex_use_all(hb_config_ex_t* config_ex)
{
	*config_ex = {HB_CONFIG_EX_MODE_ALL};
}

void hb_config_destroy(hb_config_t* opt)
{
	nh_set_int32_destroy(opt->suppressed_errors);
	free(opt);
}

bool hb_config_supressed_error_check(hb_config_t opt, hb_error_t errcode)
{
	return nh_set_int32_has(&opt->suppressed_errors, errcode);
}

bool hb_config_ex_check(hb_config_t* config, hb_proc_char_t* query)
{
	switch (config->mode) {
	case HB_CONFIG_EX_MODE_ALL:
		return true;

	case HB_CONFIG_EX_MODE_NONE:
		return false;

	default:
		return nh_set_str_has(config->set, query);
	}
	if (config->mode == HB_CONFIG_EX_MODE_ALL) {
		return true;
	}
}
