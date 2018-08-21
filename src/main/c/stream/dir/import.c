#include "../content/html.c"

void hbsd_import(hbu_streamoptions_t so, hbu_pipe_t existing_pipe, hb_char_t *parent, char *path) {
  // TODO

  hbu_fstreamin_t input = hbu_fstreamin_create(path);
  hbu_pipe_t new_pipe = hbu_pipe_create(input, (hbu_pipe_reader_cb_t) &hbu_fstreamin_read, path, existing_pipe->output, existing_pipe->writer);

  hbs_content(so, new_pipe, parent);

  // TODO
}
