#pragma once

#include <getopt.h>
#include "./stream/content/html.c"
#include "./__main__.c"

nh_set_str_t hbu_streamoptions_parse_list_of_tags(hbe_err_t *hbe_err, char *argv) {
  nh_set_str_t set = NULL;
  hb_list_charlist_t list = NULL;

  if (argv != NULL && strcmp(argv, "*")) {
    return NULL;
  }

  set = nh_set_str_create();

  if (argv == NULL) {
    return set;
  }

  list = hb_list_charlist_create_from_split((hb_proc_char_t *) argv, ',');

  for (size_t i = 0; i < list->length; i++) {
    hb_list_char_t part = hb_list_charlist_get(list, i);
    hb_proc_char_t *part_c = hb_list_char_underlying(part);

    if (hb_list_char_get(part, 0) == '$') {
      // Set of tags
      hb_list_char_shift(part);
      HBE_CATCH_F(hbu_streamoptions_parse_and_add_tag_set, (char *) part_c, set);

    } else {
      // Single tag
      if (!hb_rule_tags_check(part_c)) {
        HBE_THROW_F(HBE_CLI_INVALID_TAG, "%s is not a standard tag and was provided as part of an argument's value", part_c);
      }
      nh_set_str_add(set, (char *) hb_list_char_underlying_copy(part));
    }
  }

  finally:
    if (list != NULL) {
      hb_list_charlist_destroy_from_split(list);
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
  hb_list_charlist_t list = NULL;

  if (argv == NULL) {
    return;
  }

  list = hb_list_charlist_create_from_split((hb_proc_char_t *) argv, ',');

  for (size_t i = 0; i < list->length; i++) {
    hb_list_char_t part = hb_list_charlist_get(list, i);

    if (hb_list_char_compare_lit(part, "MALFORMED_ENTITY") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_MALFORMED_ENTITY);
    } else if (hb_list_char_compare_lit(part, "BARE_AMPERSAND") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_BARE_AMPERSAND);
    } else if (hb_list_char_compare_lit(part, "INVALID_ENTITY") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_INVALID_ENTITY);
    } else if (hb_list_char_compare_lit(part, "NONSTANDARD_TAG") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_NONSTANDARD_TAG);
    } else if (hb_list_char_compare_lit(part, "UCASE_ATTR") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_UCASE_ATTR);
    } else if (hb_list_char_compare_lit(part, "UCASE_TAG") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_UCASE_TAG);
    } else if (hb_list_char_compare_lit(part, "UNQUOTED_ATTR") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_UNQUOTED_ATTR);
    } else if (hb_list_char_compare_lit(part, "SELF_CLOSING_TAG") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_SELF_CLOSING_TAG);
    } else {
      HBE_THROW_F(HBE_CLI_INVALID_SUPPRESSABLE_ERROR, "Unrecognised suppressable error `%s`", hb_list_char_underlying(part));
    }
  }

  finally:
    if (list != NULL) {
      hb_list_charlist_destroy_from_split(list);
      list = NULL;
    }
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
    hb_rule_contenttags_add_elems(set);
  } else if (strcmp(set_name, "contentfirst") == 0) {
    hb_rule_contentfirsttags_add_elems(set);
  } else if (strcmp(set_name, "formatting") == 0) {
    hb_rule_formattingtags_add_elems(set);
  } else if (strcmp(set_name, "layout") == 0) {
    hb_rule_layouttags_add_elems(set);
  } else if (strcmp(set_name, "specific") == 0) {
    hb_rule_specifictags_add_elems(set);
  } else if (strcmp(set_name, "heading") == 0) {
    hb_rule_headingtags_add_elems(set);
  } else if (strcmp(set_name, "media") == 0) {
    hb_rule_mediatags_add_elems(set);
  } else if (strcmp(set_name, "sectioning") == 0) {
    hb_rule_sectioningtags_add_elems(set);
  } else if (strcmp(set_name, "void") == 0) {
    hb_rule_voidtags_add_elems(set);
  } else if (strcmp(set_name, "wss") == 0) {
    hb_rule_wsstags_add_elems(set);
  } else {
    HBE_THROW_V(HBE_CLI_INVALID_TAG_SET, "Unrecognised tag set `%s`", set_name);
  }
}

int main(int argc, char **argv) {
  hb_init();

  hbe_err_t err = NULL;
  hbe_err_t *hbe_err = &err;

  hbu_fstreamout_t output = NULL;
  hb_list_char_t output_buffer = NULL;

  // Prepare config
  char *input_path = NULL;
  char *output_path = NULL;
  int logging = 0;
  int config_keep = 0;
  int config_buffer = 0;
  hbu_streamoptions_t config_stream = hbu_streamoptions_create();

  int nondefault_ex_collapse_whitespace = 0;
  int nondefault_ex_destroy_whole_whitespace = 0;
  int nondefault_ex_trim_whitespace = 0;

  // Parse arguments
  while (1) {
    struct option long_options[] = {
      {"keep", no_argument, &config_keep, 1},
      {"buffer", no_argument, &config_buffer, 1},
      {"verbose", no_argument, &logging, 1},
      {"input", required_argument, NULL, 'i'},
      {"output", required_argument, NULL, 'o'},
      {"suppress", required_argument, NULL, 's'},

      {"MXcollapseWhitespace", optional_argument, NULL, 40},
      {"MXdestroyWholeWhitespace", optional_argument, NULL, 41},
      {"MXtrimWhitespace", optional_argument, NULL, 42},

      {"MXtrimClassAttr", no_argument, &(config_stream->trim_class_attr), 0},
      {"MXdecEnt", no_argument, &(config_stream->decode_entities), 0},
      {"MXcondComments", no_argument, &(config_stream->min_conditional_comments), 0},
      {"MXattrQuotes", no_argument, &(config_stream->decode_entities), 0},
      {"MXcomments", no_argument, &(config_stream->remove_comments), 0},
      {"MXoptTags", no_argument, &(config_stream->remove_optional_tags), 0},
      {"MXtagWS", no_argument, &(config_stream->remove_tag_whitespace), 0},

      {0, 0, 0, 0}
    };

    int option_index = 0;
    int c = getopt_long(argc, argv, "kbvi:o:s:", long_options, &option_index);

    if (c == -1) {
      if (optind != argc) {
        HBE_THROW_F(HBE_CLI_TOO_MANY_OPTIONS, "Too many arguments provided");
      }
      break;
    }

    switch (c) {
    case 'i':
      input_path = optarg;
      break;

    case 'o':
      output_path = optarg;
      break;

    case 's':
      HBE_CATCH_F(hbu_streamoptions_parse_and_add_errors_to_suppress, config_stream->suppressed_errors, optarg);
      break;

    case 40:
      nondefault_ex_collapse_whitespace = 1;
      config_stream->ex_collapse_whitespace = HBE_CATCH_F(hbu_streamoptions_parse_list_of_tags, optarg);
      break;

    case 41:
      nondefault_ex_destroy_whole_whitespace = 1;
      config_stream->ex_destroy_whole_whitespace = HBE_CATCH_F(hbu_streamoptions_parse_list_of_tags, optarg);
      break;

    case 42:
      nondefault_ex_trim_whitespace = 1;
      config_stream->ex_trim_whitespace = HBE_CATCH_F(hbu_streamoptions_parse_list_of_tags, optarg);
      break;
    }
  }

  if (!nondefault_ex_collapse_whitespace) config_stream->ex_collapse_whitespace = hbu_streamoptions_default_ex_collapse_whitespace();
  if (!nondefault_ex_destroy_whole_whitespace) config_stream->ex_destroy_whole_whitespace = hbu_streamoptions_default_ex_destroy_whole_whitespace();
  if (!nondefault_ex_trim_whitespace) config_stream->ex_trim_whitespace = hbu_streamoptions_default_ex_trim_whitespace();

  if (logging) {
    hbl_info_kv_string("Input", input_path);
    hbl_info_kv_string("Output", output_path);
    hbl_info_kv_boolean("Buffer output until success", config_buffer);
    hbl_info_kv_boolean("Keep output file on error", config_keep);
    hbu_streamoptions_log(config_stream);
  }

  hb_proc_t pipe = hb_proc_create_blank(input_path);

  hbu_fstreamin_t input = HBE_CATCH_F(hbu_fstreamin_create, input_path);
  hb_proc_blank_set_input_fstreamin(pipe, input);

  if (config_buffer) {
    output_buffer = hb_list_char_create();
    hb_proc_blank_set_output_buffer(pipe, output_buffer);
  } else {
    output = HBE_CATCH_F(hbu_fstreamout_create, output_path);
    hb_proc_blank_set_output_fstreamout(pipe, output);
  }

  HBE_CATCH_F(hbs_content, config_stream, pipe, NULL);

  if (config_buffer) {
    output = HBE_CATCH_F(hbu_fstreamout_create, output_path);
    HBE_CATCH_F(hbu_fstreamout_write_buffer, output, output_buffer);
  }

  finally:
    if (err != NULL) {
      hbl_error(err);
      if (output != NULL && !config_keep && !config_buffer) {
        // Delete only after opening output stream (don't delete before existing file has not been touched)
        // Don't need to set if $config_buffer, as it won't write anything anyway
        if (unlink(output_path)) {
          hbl_log(HBL_LOG_WARN, "Failed to delete file %s with error %d", output_path, errno);
        } else {
          hbl_log(HBL_LOG_INFO, "%s has been deleted", output_path);
        }
      }
      exit(err->code);
    }

    if (logging) {
      hbl_log(HBL_LOG_INFO, "All done!");
    }
    exit(0);
}
