#ifndef _HDR_HYPERBUILD_STREAM_OPTIONS
#define _HDR_HYPERBUILD_STREAM_OPTIONS

#include "../error/error.c"
#include "../ext/nicehash/set/str.h"

typedef struct hbs_options_s {
  nh_set_str_t ex_collapse_whitespace; // Could be NULL to represent the universal set (i.e. don't enable)
  nh_set_str_t ex_destroy_whole_whitespace; // Could be NULL to represent the universal set (i.e. don't enable)
  nh_set_str_t ex_trim_whitespace; // Could be NULL to represent the universal set (i.e. don't enable)
  int trim_class_attr;
  int decode_entities;
  int min_conditional_comments;
  int remove_attr_quotes;
  int remove_comments;
  int remove_optional_tags;
  int remove_tag_whitespace;
} *hbs_options_t;

static nh_set_str_t _hbs_options_default_ex_collapse_whitespace(void) {
  nh_set_str_t set = nh_set_str_create();
  hbr_wsstags_add_elems(set);
  return set;
}

static nh_set_str_t _hbs_options_default_ex_destroy_whole_whitespace(void) {
  nh_set_str_t set = nh_set_str_create();
  hbr_wsstags_add_elems(set);
  hbr_contenttags_add_elems(set);
  hbr_formattingtags_add_elems(set);
  return set;
}

static nh_set_str_t _hbs_options_default_ex_trim_whitespace(void) {
  nh_set_str_t set = nh_set_str_create();
  hbr_wsstags_add_elems(set);
  hbr_formattingtags_add_elems(set);
  return set;
}

// WARNING: Rules must be initialised before calling this function
hbs_options_t hbs_options_create(void) {
  hbs_options_t opt = malloc(sizeof(struct hbs_options_s));
  opt->ex_collapse_whitespace = _hbs_options_default_ex_collapse_whitespace();
  opt->ex_destroy_whole_whitespace = _hbs_options_default_ex_destroy_whole_whitespace();
  opt->ex_trim_whitespace = _hbs_options_default_ex_trim_whitespace();
  opt->trim_class_attr = 1;
  opt->decode_entities = 1;
  opt->min_conditional_comments = 1;
  opt->remove_attr_quotes = 1;
  opt->remove_comments = 1;
  opt->remove_optional_tags = 1;
  opt->remove_tag_whitespace = 1;
  return opt;
}

void hbs_options_log(hbs_options_t opt) {
  hbe_info_kv_boolean("Trim `class` attributes", opt->trim_class_attr);
  hbe_info_kv_boolean("Decode entities", opt->decode_entities);
  hbe_info_kv_boolean("Minify conditional comments", opt->min_conditional_comments);
  hbe_info_kv_boolean("Remove attribute quotes", opt->remove_attr_quotes);
  hbe_info_kv_boolean("Remove comments", opt->remove_comments);
  hbe_info_kv_boolean("Remove optional tags", opt->remove_optional_tags);
  hbe_info_kv_boolean("Remove tag whitespace", opt->remove_tag_whitespace);
}

#endif // _HDR_HYPERBUILD_STREAM_OPTIONS
