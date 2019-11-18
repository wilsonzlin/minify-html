#pragma once

typedef enum {
	HB_CONFIG_EX_MODE_NONE,    // i.e. minify all without exeption
	HB_CONFIG_EX_MODE_DEFAULT, // entire struct will not be destroyed
	HB_CONFIG_EX_MODE_CUSTOM,  // set will be destroyed
	HB_CONFIG_EX_MODE_ALL,     // i.e. don't minify
} hb_config_ex_mode_t;

typedef struct {
	hb_config_ex_mode_t mode;
	nh_set_str set;
} hb_config_ex_t;

typedef struct {
	hb_config_ex_t ex_collapse_whitespace;
	hb_config_ex_t ex_destroy_whole_whitespace;
	hb_config_ex_t ex_trim_whitespace;
	nh_set_int32 suppressed_errors;
	bool trim_class_attributes;
	bool decode_entities;
	bool remove_attr_quotes;
	bool remove_comments;
	bool remove_optional_tags;
	bool remove_tag_whitespace;
} hb_config_t;

// WARNING: Rules must be initialised before calling this function
void hb_config_init(void);
hb_config_t* hb_config_create(void);
void hb_config_ex_use_none(hb_config_ex_t* config_ex);
void hb_config_ex_use_custom(hb_config_ex_t* config_ex, nh_set_str custom_set);
void hb_config_ex_use_all(hb_config_ex_t* config_ex);
void hb_config_destroy(hb_config_t* opt);
bool hb_config_supressed_error_check(hb_config_t opt, hb_error_t errcode);
bool hb_config_ex_check(hb_config_ex_t* config, hb_proc_char_t* query);
