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
  hb_r_init();

  // Prepare config
  char *input_path = NULL;
  char *output_path = NULL;

  // Parse arguments
  while (1) {
    static struct option long_options[] = {
      {"input", required_argument, NULL, 'i'},
      {"output", required_argument, NULL, 'o'},
      {0, 0, 0, 0}
    };

    int option_index = 0;
    int c = getopt_long(argc, argv, "i:o:", long_options, &option_index);

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
    }
  }

  hbe_debug("Input: %s", input_path);
  hbe_debug("Output: %s", output_path);

  hbu_fstreamin_t input = hbu_fstreamin_create(input_path);
  hbu_fstreamout_t output = hbu_fstreamout_create(output_path);

  if (output != NULL) {
    // Set after opening output stream (file is created then)
    hbe_fatal_set_autodelete(output_path);
  }

  hbu_pipe_t pipe = hbu_pipe_create_blank();
  hbu_pipe_blank_set_input(pipe, input);
  hbu_pipe_blank_set_output_fstreamout(pipe, output);

  hbs_content(pipe);

  hbe_debug("All done!");
  exit(EXIT_SUCCESS);
}
