#ifndef _HDR_HYPERBUILD_STREAM_OPTIONS
#define _HDR_HYPERBUILD_STREAM_OPTIONS

typedef struct hbs_options_s {
  nh_set_str_t ex_collapse_whitespace;
  nh_set_str_t ex_destroy_whole_whitespace;
  nh_set_str_t ex_trim_whitespace;
  int trim_class_attr;
  int decode_entities;
  int min_conditional_comments;
  int remove_attr_quotes;
  int remove_optional_tags;
  int remove_tag_whitespace;
} *hbs_options_t;

#endif // _HDR_HYPERBUILD_STREAM_OPTIONS
