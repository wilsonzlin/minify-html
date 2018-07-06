#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include "error/error.c"

#include "rule/init.c"

#include "util/fstreamin.c"
#include "util/fstreamout.c"
#include "util/pipe.c"

#include "stream/content.c"

int main(int argc, char **argv) {
  // Set up rules
  hbr_init();

  // Prepare config
  char *input_path = NULL;
  char *output_path = NULL;
  int config_keep = 0;
  int config_buffer = 0;

  // Parse arguments
  while (1) {
    static struct option long_options[] = {
      {"keep", no_argument, NULL, 'k'},
      {"buffer", no_argument, NULL, 'b'},
      {"input", required_argument, NULL, 'i'},
      {"output", required_argument, NULL, 'o'},
      {0, 0, 0, 0}
    };

    int option_index = 0;
    int c = getopt_long(argc, argv, "kbi:o:", long_options, &option_index);

    if (c == -1) {
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
    }
  }

  hbe_debug("Input: %s", input_path);
  hbe_debug("Output: %s", output_path);
  if (config_buffer) {
    hbe_debug("Buffer: %d", config_buffer);
  }
  if (config_keep) {
    hbe_debug("Keep: %d", config_keep);
  }

  hbu_pipe_t pipe = hbu_pipe_create_blank();

  hbu_fstreamin_t input = hbu_fstreamin_create(input_path);
  hbu_pipe_blank_set_input(pipe, input);

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

  hbs_content(pipe);

  if (config_buffer) {
    output = hbu_fstreamout_create(output_path);
    hbu_fstreamout_write_buffer(output, output_buffer);
  }

  hbe_debug("All done!");
  exit(EXIT_SUCCESS);
}
