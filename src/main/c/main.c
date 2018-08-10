#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include "error/error.c"

#include "rule/init.c"

#include "ext/nicehash/set/str.h"
#include "ext/nicehash/set/int32.h"
#include "datastructure/list/buffer.h"

#include "util/fstreamin.c"
#include "util/fstreamout.c"
#include "util/pipe.c"

#include "stream/streamoptions.c"
#include "stream/content.c"

static void _add_tag_set(char *set_name, nh_set_str_t set) {
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
    hbe_fatal(HBE_CLI_INVALID_TAG_SET, "Unrecognised tag set `%s`", set_name);
  }
}

static nh_set_str_t _parse_list_of_tags(char *argv) {
  if (argv != NULL && strcmp(argv, "*")) {
    return NULL;
  }

  nh_set_str_t set = nh_set_str_create();

  if (argv == NULL) {
    return set;
  }

  hb_bufferlist_t list = hb_bufferlist_create_from_split((hb_char_t *) argv, ',');

  for (size_t i = 0; i < list->length; i++) {
    hbu_buffer_t part = hb_bufferlist_get(list, i);
    hb_char_t *part_c = hbu_buffer_underlying(part);

    if (hbu_buffer_get(part, 0) == '$') {
      // Set of tags
      hbu_buffer_shift(part);
      _add_tag_set((char *) part_c, set);

    } else {
      // Single tag
      if (!hbr_tags_check(part_c)) {
        hbe_fatal(HBE_CLI_INVALID_TAG, "%s is not a standard tag and was provided as part of an argument's value", part_c);
      }
      nh_set_str_add(set, (char *) hbu_buffer_underlying_copy(part));
    }
  }

  hb_bufferlist_destroy_from_split(list);

  return set;
}

static void _parse_and_add_errors_to_suppress(nh_set_int32_t suppressed_errors, char *argv) {
  if (argv == NULL) {
    return;
  }

  hb_bufferlist_t list = hb_bufferlist_create_from_split((hb_char_t *) argv, ',');

  for (size_t i = 0; i < list->length; i++) {
    hbu_buffer_t part = hb_bufferlist_get(list, i);

    if (hbu_buffer_compare_lit(part, "MALFORMED_ENTITY") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_MALFORMED_ENTITY);
    } else if (hbu_buffer_compare_lit(part, "BARE_AMPERSAND") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_BARE_AMPERSAND);
    } else if (hbu_buffer_compare_lit(part, "INVALID_ENTITY") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_INVALID_ENTITY);
    } else if (hbu_buffer_compare_lit(part, "NONSTANDARD_TAG") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_NONSTANDARD_TAG);
    } else if (hbu_buffer_compare_lit(part, "UCASE_ATTR") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_UCASE_ATTR);
    } else if (hbu_buffer_compare_lit(part, "UCASE_TAG") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_UCASE_TAG);
    } else if (hbu_buffer_compare_lit(part, "UNQUOTED_ATTR") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_UNQUOTED_ATTR);
    } else if (hbu_buffer_compare_lit(part, "SELF_CLOSING_TAG") == 0) {
      nh_set_int32_add(suppressed_errors, HBE_PARSE_SELF_CLOSING_TAG);
    } else {
      hbe_fatal(HBE_CLI_INVALID_SUPPRESSABLE_ERROR, "Unrecognised suppressable error `%s`", hbu_buffer_underlying(part));
    }
  }

  hb_bufferlist_destroy_from_split(list);
}

int main(int argc, char **argv) {
  // Set up rules
  hbr_init();

  // Prepare config
  char *input_path = NULL;
  char *output_path = NULL;
  int config_keep = 0;
  int config_buffer = 0;
  hbs_options_t config_stream = hbs_options_create();

  int nondefault_ex_collapse_whitespace = 0;
  int nondefault_ex_destroy_whole_whitespace = 0;
  int nondefault_ex_trim_whitespace = 0;

  // Parse arguments
  while (1) {
    struct option long_options[] = {
      {"keep", no_argument, NULL, 'k'},
      {"buffer", no_argument, NULL, 'b'},
      {"verbose", no_argument, NULL, 'v'},
      {"input", required_argument, NULL, 'i'},
      {"output", required_argument, NULL, 'o'},
      {"suppress", required_argument, NULL, 's'},

      {"MXcollapseWhitespace", optional_argument, NULL, 1},
      {"MXdestroyWholeWhitespace", optional_argument, NULL, 2},
      {"MXtrimWhitespace", optional_argument, NULL, 3},

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
        hbe_fatal(HBE_CLI_TOO_MANY_OPTIONS, "Too many arguments provided");
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

    case 'k':
      config_keep = 1;
      break;

    case 'b':
      config_buffer = 1;
      break;

    case 'v':
      hbe_info_toggle(1);
      break;

    case 's':
      _parse_and_add_errors_to_suppress(config_stream->suppressed_errors, optarg);
      break;

    case 1:
      nondefault_ex_collapse_whitespace = 1;
      config_stream->ex_collapse_whitespace = _parse_list_of_tags(optarg);
      break;

    case 2:
      nondefault_ex_destroy_whole_whitespace = 1;
      config_stream->ex_destroy_whole_whitespace = _parse_list_of_tags(optarg);
      break;

    case 3:
      nondefault_ex_trim_whitespace = 1;
      config_stream->ex_trim_whitespace = _parse_list_of_tags(optarg);
      break;
    }
  }

  if (!nondefault_ex_collapse_whitespace) config_stream->ex_collapse_whitespace = hbs_options_default_ex_collapse_whitespace();
  if (!nondefault_ex_destroy_whole_whitespace) config_stream->ex_destroy_whole_whitespace = hbs_options_default_ex_destroy_whole_whitespace();
  if (!nondefault_ex_trim_whitespace) config_stream->ex_trim_whitespace = hbs_options_default_ex_trim_whitespace();

  hbe_info_kv_string("Input", input_path);
  hbe_info_kv_string("Output", output_path);
  hbe_info_kv_boolean("Buffer output until success", config_buffer);
  hbe_info_kv_boolean("Keep output file on error", config_keep);
  hbs_options_log(config_stream);

  hbu_pipe_t pipe = hbu_pipe_create_blank(input_path);

  hbu_fstreamin_t input = hbu_fstreamin_create(input_path);
  hbu_pipe_blank_set_input_fstreamin(pipe, input);

  hbu_fstreamout_t output;
  hbu_buffer_t output_buffer;

  if (config_buffer) {
    output = NULL;
    output_buffer = hbu_buffer_create();
    hbu_pipe_blank_set_output_buffer(pipe, output_buffer);
  } else {
    output = hbu_fstreamout_create(output_path);
    output_buffer = NULL;
    hbu_pipe_blank_set_output_fstreamout(pipe, output);
  }

  if (output != NULL && !config_keep && !config_buffer) {
    // Set after opening output stream (file is created then)
    // Don't need to set if $config_buffer, as it won't write anything anyway
    hbe_fatal_set_autodelete(output_path);
  }

  hbs_content(config_stream, pipe, NULL);

  if (config_buffer) {
    output = hbu_fstreamout_create(output_path);
    hbu_fstreamout_write_buffer(output, output_buffer);
  }

  hbe_info("All done!");
  exit(EXIT_SUCCESS);
}
