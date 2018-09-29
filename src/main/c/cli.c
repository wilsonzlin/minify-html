#include <getopt.h>
#include "./stream/content/html.c"
#include "./__main__.c"

int main(int argc, char **argv) {
  hb_init();

  hbe_err_t err = NULL;
  hbe_err_t *hbe_err = &err;

  hbu_fstreamout_t output = NULL;
  hbu_list_char_t output_buffer = NULL;

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

  hbu_pipe_t pipe = hbu_pipe_create_blank(input_path);

  hbu_fstreamin_t input = HBE_CATCH_F(hbu_fstreamin_create, input_path);
  hbu_pipe_blank_set_input_fstreamin(pipe, input);

  if (config_buffer) {
    output_buffer = hbu_list_char_create();
    hbu_pipe_blank_set_output_buffer(pipe, output_buffer);
  } else {
    output = HBE_CATCH_F(hbu_fstreamout_create, output_path);
    hbu_pipe_blank_set_output_fstreamout(pipe, output);
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
