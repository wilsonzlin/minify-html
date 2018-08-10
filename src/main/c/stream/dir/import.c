#ifndef _HDR_HYPERBUILD_STREAM_DIR_IMPORT
#define _HDR_HYPERBUILD_STREAM_DIR_IMPORT

#include "../../util/hbchar.h"
#include "../../util/pipe.c"

#include "../streamoptions.c"
#include "../content.c"

void hbsd_import(hbs_options_t so, hbu_pipe_t existing_pipe, hb_char_t *parent, char *path) {
  // TODO
  
  hbu_fstreamin_t input = hbu_fstreamin_create(path);
  hbu_pipe_t new_pipe = hbu_pipe_create(input, (hbu_pipe_reader_cb_t) &hbu_fstreamin_read, path, existing_pipe->output, existing_pipe->writer);

  hbs_content(so, new_pipe, parent);

  // TODO
}

#endif // _HDR_HYPERBUILD_STREAM_DIR_IMPORT
