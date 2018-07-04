#include <stddef.h>
#include <stdio.h>
#include <getopt.h>
#include "error/error.c"
#include "util/fstreamin.c"
#include "util/fstreamout.c"
#include "util/pipe.c"

int main(int argc, char **argv) {
  char *input_path = NULL;
  char *output_path = NULL;

  int c;

  while (1) {
    static struct option long_options[] = {
      {"input", required_argument, NULL, 'i'},
      {"output", required_argument, NULL, 'o'},
      {0, 0, 0, 0}
    };

    int option_index = 0;
    c = getopt_long(argc, argv, "i:o:", long_options, &option_index);

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

  if (input_path == NULL) {
    hbe_fatal("No input file provided");
  }

  if (output_path == NULL) {
    hbe_fatal("No output file provided");
  }

  printf("Input: %s\n", input_path);
  printf("Output: %s\n", output_path);

  hbu_fstreamin_t input = hbu_fstreamin_create(input_path);
  hbu_fstreamout_t output = hbu_fstreamout_create(output_path);

  hbu_pipe_t pipe = hbu_pipe_create_blank();
  hbu_pipe_blank_set_input(pipe, input);
  hbu_pipe_blank_set_output_fstreamout(pipe, output);

  while (1) {
    hbu_pipe_accept(pipe);
  }
}
