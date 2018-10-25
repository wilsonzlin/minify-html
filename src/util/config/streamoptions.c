#include "../execution/error.c"
#include "../execution/logging.c"
#include "../char/char.c"
#include "../list/char.c"
#include "../list/charlist.c"
#include "../../rule/tag/contentfirsttags.c"
#include "../../rule/tag/contenttags.c"
#include "../../rule/tag/formattingtags.c"
#include "../../rule/tag/headingtags.c"
#include "../../rule/tag/layouttags.c"
#include "../../rule/tag/mediatags.c"
#include "../../rule/tag/sectioningtags.c"
#include "../../rule/tag/specifictags.c"
#include "../../rule/tag/tags.c"
#include "../../rule/tag/voidtags.c"
#include "../../rule/tag/wsstags.c"

typedef struct hbu_streamoptions_s {
  nh_set_str_t ex_collapse_whitespace; // Could be NULL to represent the universal set (i.e. don't minify)
  nh_set_str_t ex_destroy_whole_whitespace; // Could be NULL to represent the universal set (i.e. don't minify)
  nh_set_str_t ex_trim_whitespace; // Could be NULL to represent the universal set (i.e. don't minify)
  nh_set_int32_t suppressed_errors;
  int trim_class_attr;
  int decode_entities;
  int min_conditional_comments;
  int remove_attr_quotes;
  int remove_comments;
  int remove_optional_tags;
  int remove_tag_whitespace;
} *hbu_streamoptions_t;

static nh_set_str_t _hbu_streamoptions_default_ex_collapse_whitespace = NULL;
nh_set_str_t hbu_streamoptions_default_ex_collapse_whitespace(void) {
  if (_hbu_streamoptions_default_ex_collapse_whitespace == NULL) {
    _hbu_streamoptions_default_ex_collapse_whitespace = nh_set_str_create();
    hbr_wsstags_add_elems(_hbu_streamoptions_default_ex_collapse_whitespace);
  }
  return _hbu_streamoptions_default_ex_collapse_whitespace;
}

static nh_set_str_t _hbu_streamoptions_default_ex_destroy_whole_whitespace = NULL;
nh_set_str_t hbu_streamoptions_default_ex_destroy_whole_whitespace(void) {
  if (_hbu_streamoptions_default_ex_destroy_whole_whitespace == NULL) {
    _hbu_streamoptions_default_ex_destroy_whole_whitespace = nh_set_str_create();
    hbr_wsstags_add_elems(_hbu_streamoptions_default_ex_destroy_whole_whitespace);
    hbr_contenttags_add_elems(_hbu_streamoptions_default_ex_destroy_whole_whitespace);
    hbr_formattingtags_add_elems(_hbu_streamoptions_default_ex_destroy_whole_whitespace);
  }
  return _hbu_streamoptions_default_ex_destroy_whole_whitespace;
}

static nh_set_str_t _hbu_streamoptions_default_ex_trim_whitespace = NULL;
nh_set_str_t hbu_streamoptions_default_ex_trim_whitespace(void) {
  if (_hbu_streamoptions_default_ex_trim_whitespace == NULL) {
    _hbu_streamoptions_default_ex_trim_whitespace = nh_set_str_create();
    hbr_wsstags_add_elems(_hbu_streamoptions_default_ex_trim_whitespace);
    hbr_formattingtags_add_elems(_hbu_streamoptions_default_ex_trim_whitespace);
  }
  return _hbu_streamoptions_default_ex_trim_whitespace;
}

// WARNING: Rules must be initialised before calling this function
hbu_streamoptions_t hbu_streamoptions_create(void) {
  hbu_streamoptions_t opt = malloc(sizeof(struct hbu_streamoptions_s));
  opt->ex_collapse_whitespace = NULL;
  opt->ex_destroy_whole_whitespace = NULL;
  opt->ex_trim_whitespace = NULL;
  opt->suppressed_errors = nh_set_int32_create();
  opt->trim_class_attr = 1;
  opt->decode_entities = 1;
  opt->min_conditional_comments = 1;
  opt->remove_attr_quotes = 1;
  opt->remove_comments = 1;
  opt->remove_optional_tags = 1;
  opt->remove_tag_whitespace = 1;
  return opt;
}

// WARNING: Does not destroy ex_* members
void hbu_streamoptions_destroy(hbu_streamoptions_t opt) {
  nh_set_int32_destroy(opt->suppressed_errors);
  free(opt);
}

int hbu_streamoptions_in_tags_list(nh_set_str_t set, hb_char_t *query) {
  if (set == NULL) {
    return 1; // NULL represents the universal set
  }

  if (query == NULL) {
    // When parent is #root
    return 0;
  }

  return nh_set_str_has(set, (char *) query);
}

int hbu_streamoptions_supressed_error(hbu_streamoptions_t opt, hbe_errcode_t errcode) {
  return nh_set_int32_has(opt->suppressed_errors, errcode);
}

void hbu_streamoptions_log(hbu_streamoptions_t opt) {
  hbl_info_kv_boolean("Trim `class` attributes", opt->trim_class_attr);
  hbl_info_kv_boolean("Decode entities", opt->decode_entities);
  hbl_info_kv_boolean("Minify conditional comments", opt->min_conditional_comments);
  hbl_info_kv_boolean("Remove attribute quotes", opt->remove_attr_quotes);
  hbl_info_kv_boolean("Remove comments", opt->remove_comments);
  hbl_info_kv_boolean("Remove optional tags", opt->remove_optional_tags);
  hbl_info_kv_boolean("Remove tag whitespace", opt->remove_tag_whitespace);
}

void hbu_streamoptions_parse_and_add_tag_set(hbe_err_t *hbe_err, char *set_name, nh_set_str_t set) {
  if (strcmp(set_name, "content") == 0) {
    hbr_contenttags_add_elems(set);
  } else if (strcmp(set_name, "contentfirst") == 0) {
    hbr_contentfirsttags_add_elems(set);
  } else if (strcmp(set_name, "formatting") == 0) {
    hbr_formattingtags_add_elems(set);
  } else if (strcmp(set_name, "layout") == 0) {
    hbr_layouttags_add_elems(set);
  } else if (strcmp(set_name, "specific") == 0) {
    hbr_specifictags_add_elems(set);
  } else if (strcmp(set_name, "heading") == 0) {
    hbr_headingtags_add_elems(set);
  } else if (strcmp(set_name, "media") == 0) {
    hbr_mediatags_add_elems(set);
  } else if (strcmp(set_name, "sectioning") == 0) {
    hbr_sectioningtags_add_elems(set);
  } else if (strcmp(set_name, "void") == 0) {
    hbr_voidtags_add_elems(set);
  } else if (strcmp(set_name, "wss") == 0) {
    hbr_wsstags_add_elems(set);
  } else {
    HBE_THROW_V(HBE_CLI_INVALID_TAG_SET, "Unrecognised tag set `%s`", set_name);
  }
}

nh_set_str_t hbu_streamoptions_parse_list_of_tags(hbe_err_t *hbe_err, char *argv) {
  nh_set_str_t set = NULL;
  hbu_list_charlist_t list = NULL;

  if (argv != NULL && strcmp(argv, "*")) {
    return NULL;
  }

  set = nh_set_str_create();

  if (argv == NULL) {
    return set;
  }

  list = hbu_list_charlist_create_from_split((hb_char_t *) argv, ',');

  for (size_t i = 0; i < list->length; i++) {
    hbu_list_char_t part = hbu_list_charlist_get(list, i);
    hb_char_t *part_c = hbu_list_char_underlying(part);

    if (hbu_list_char_get(part, 0) == '$') {
      // Set of tags
      hbu_list_char_shift(part);
      HBE_CATCH_F(hbu_streamoptions_parse_and_add_tag_set, (char *) part_c, set);

    } else {
      // Single tag
      if (!hbr_tags_check(part_c)) {
        HBE_THROW_F(HBE_CLI_INVALID_TAG, "%s is not a standard tag and was provided as part of an argument's value", part_c);
      }
      nh_set_str_add(set, (char *) hbu_list_char_underlying_copy(part));
    }
  }

  finally:
    if (list != NULL) {
      hbu_list_charlist_destroy_from_split(list);
      list = NULL;
    }
    if (*hbe_err != NULL) {
      if (set != NULL) {
        nh_set_str_destroy(set);
        set = NULL;
      }
    }
    return set;
}

void hbu_streamoptions_parse_and_add_errors_to_suppress(hbe_err_t *hbe_err, nh_set_int32_t suppressed_errors, char *argv) {
  hbu_list_charlist_t list = NULL;

  if (argv == NULL) {
    return;
  }

  list = hbu_list_charlist_create_from_split((hb_char_t *) argv, ',');

  for (size_t i = 0; i < list->length; i++) {
    hbu_list_char_t part = hbu_list_charlist_get(list, i);

    if (hbu_list_char_compare_lit(part, "MALFORMED_ENTITY") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_MALFORMED_ENTITY);
    } else if (hbu_list_char_compare_lit(part, "BARE_AMPERSAND") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_BARE_AMPERSAND);
    } else if (hbu_list_char_compare_lit(part, "INVALID_ENTITY") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_INVALID_ENTITY);
    } else if (hbu_list_char_compare_lit(part, "NONSTANDARD_TAG") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_NONSTANDARD_TAG);
    } else if (hbu_list_char_compare_lit(part, "UCASE_ATTR") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_UCASE_ATTR);
    } else if (hbu_list_char_compare_lit(part, "UCASE_TAG") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_UCASE_TAG);
    } else if (hbu_list_char_compare_lit(part, "UNQUOTED_ATTR") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_UNQUOTED_ATTR);
    } else if (hbu_list_char_compare_lit(part, "SELF_CLOSING_TAG") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_SELF_CLOSING_TAG);
    } else {
      HBE_THROW_F(HBE_CLI_INVALID_SUPPRESSABLE_ERROR, "Unrecognised suppressable error `%s`", hbu_list_char_underlying(part));
    }
  }

  finally:
    if (list != NULL) {
      hbu_list_charlist_destroy_from_split(list);
      list = NULL;
    }
}
