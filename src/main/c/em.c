#include "./stream/content/html.c"
#include "./__main__.c"

void em_init(void) {
  hb_init();
}

hbe_err_t em_entry(
  // Can be NULL
  char *input_code,
  size_t input_code_length,

  // Can be NULL
  char *input_path,

  // Can be NULL
  char **output_code,
  // Can be NULL
  size_t *output_code_length,

  // Can be NULL
  char *output_path,

  int config_keep,
  int config_buffer,

  // Can be NULL
  char *suppress,

  // Need nondefault_ex_* because NULL is a valid value for ex_*
  int nondefault_ex_collapse_whitespace,
  char *ex_collapse_whitespace,
  int nondefault_ex_destroy_whole_whitespace,
  char *ex_destroy_whole_whitespace,
  int nondefault_ex_trim_whitespace,
  char *ex_trim_whitespace,

  int trim_class_attr,
  int decode_entities,
  int min_conditional_comments,
  int remove_attr_quotes,
  int remove_comments,
  int remove_optional_tags,
  int remove_tag_whitespace
) {
  em_init();

  hbe_err_t err = NULL;
  hbe_err_t *hbe_err = &err;

  hbu_fstreamin_t input_file = NULL;
  hbu_list_char_t input_buffer = NULL;

  hbu_fstreamout_t output_file = NULL;
  hbu_list_char_t output_buffer = NULL;

  hbu_streamoptions_t config_stream = hbu_streamoptions_create();
  hbu_pipe_t pipe = NULL;

  nh_set_str_t ex_collapse_whitespace_set = NULL;
  nh_set_str_t ex_destroy_whole_whitespace_set = NULL;
  nh_set_str_t ex_trim_whitespace_set = NULL;

  if (suppress != NULL) {
    HBE_CATCH_F(hbu_streamoptions_parse_and_add_errors_to_suppress, config_stream->suppressed_errors, suppress);
  }

  if (nondefault_ex_collapse_whitespace) {
    ex_collapse_whitespace_set = HBE_CATCH_F(hbu_streamoptions_parse_list_of_tags, ex_collapse_whitespace);
    config_stream->ex_collapse_whitespace = ex_collapse_whitespace_set;
  } else {
    config_stream->ex_collapse_whitespace = hbu_streamoptions_default_ex_collapse_whitespace();
  }

  if (nondefault_ex_destroy_whole_whitespace) {
    ex_destroy_whole_whitespace_set = HBE_CATCH_F(hbu_streamoptions_parse_list_of_tags, ex_destroy_whole_whitespace);
    config_stream->ex_destroy_whole_whitespace = ex_destroy_whole_whitespace_set;
  } else {
    config_stream->ex_destroy_whole_whitespace = hbu_streamoptions_default_ex_destroy_whole_whitespace();
  }

  if (nondefault_ex_trim_whitespace) {
    ex_trim_whitespace_set = HBE_CATCH_F(hbu_streamoptions_parse_list_of_tags, ex_trim_whitespace);
    config_stream->ex_trim_whitespace = ex_trim_whitespace_set;
  } else {
    config_stream->ex_trim_whitespace = hbu_streamoptions_default_ex_trim_whitespace();
  }

  config_stream->trim_class_attr = trim_class_attr;
  config_stream->decode_entities = decode_entities;
  config_stream->min_conditional_comments = min_conditional_comments;
  config_stream->remove_attr_quotes = remove_attr_quotes;
  config_stream->remove_comments = remove_comments;
  config_stream->remove_optional_tags = remove_optional_tags;
  config_stream->remove_tag_whitespace = remove_tag_whitespace;

  pipe = hbu_pipe_create_blank(input_path);

  if (input_code != NULL) {
    // Use provided code as input
    input_buffer = hbu_list_char_create();
    hbu_list_char_extend_arr(input_buffer, (hb_char_t *) input_code, input_code_length);
    hbu_pipe_blank_set_input_buffer(pipe, input_buffer);
  } else {
    // Read from a file as input
    input_file = HBE_CATCH_F(hbu_fstreamin_create, input_path);
    hbu_pipe_blank_set_input_fstreamin(pipe, input_file);
  }

  if (config_buffer || input_code != NULL) {
    // Direct output to a buffer if --buffer or input is code (not file)
    output_buffer = hbu_list_char_create();
    hbu_pipe_blank_set_output_buffer(pipe, output_buffer);
  } else {
    // Direct output to a file
    output_file = HBE_CATCH_F(hbu_fstreamout_create, output_path);
    hbu_pipe_blank_set_output_fstreamout(pipe, output_file);
  }

  // Magic
  HBE_CATCH_F(hbs_content, config_stream, pipe, NULL);

  if (output_code != NULL) {
    // Send back pointer to underlying data in output buffer
    *output_code = (char *) hbu_list_char_underlying(output_buffer);
    *output_code_length = output_buffer->length;
    hbu_list_char_destroy_shallow(output_buffer);
    output_buffer = NULL;
  } else if (config_buffer) {
    // Write buffered output data to file
    output_file = HBE_CATCH_F(hbu_fstreamout_create, output_path);
    HBE_CATCH_F(hbu_fstreamout_write_buffer, output_file, output_buffer);
  }

  finally:
    if (err != NULL) {
      if (output_file != NULL && input_buffer == NULL && !config_keep) {
        // Delete only after opening output stream (don't delete before existing file has not been touched)
        // Don't need to set if $config_buffer, as it won't write anything anyway
        if (unlink(output_path)) {
          hbl_log(HBL_LOG_WARN, "Failed to delete file %s with error %d", output_path, errno);
        }
      }
    }

    // Don't overwrite any existing err
    if (input_file != NULL) {
      hbe_err_t fstreamin_destroy_error = NULL;
      hbu_fstreamin_destroy(&fstreamin_destroy_error, input_file);
      if (fstreamin_destroy_error != NULL) {
        hbl_error(fstreamin_destroy_error);
      }
    }

    if (input_buffer != NULL) {
      hbu_list_char_destroy(input_buffer);
    }

    // Don't overwrite any existing err
    if (output_file != NULL) {
      hbe_err_t fstreamout_destroy_error = NULL;
      hbu_fstreamout_destroy(&fstreamout_destroy_error, output_file);
      if (fstreamout_destroy_error != NULL) {
        hbl_error(fstreamout_destroy_error);
      }
    }

    if (output_buffer != NULL) {
      hbu_list_char_destroy(output_buffer);
    }

    hbu_streamoptions_destroy(config_stream);

    if (pipe != NULL) {
      hbu_pipe_destroy(pipe);
    }

    if (ex_collapse_whitespace_set != NULL) {
      nh_set_str_destroy(ex_collapse_whitespace_set);
    }

    if (ex_destroy_whole_whitespace_set != NULL) {
      nh_set_str_destroy(ex_destroy_whole_whitespace_set);
    }

    if (ex_trim_whitespace_set != NULL) {
      nh_set_str_destroy(ex_trim_whitespace_set);
    }

    return err;
}
