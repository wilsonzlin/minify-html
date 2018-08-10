#ifndef _HDR_HYPERBUILD_STREAM_DIR
#define _HDR_HYPERBUILD_STREAM_DIR

#include "../util/pipe.c"

#include "./streamoptions.c"

static void _hbs_dir_eval(hbs_options_t so, hbu_pipe_t pipe, hb_char_t *name, hb_char_t **arg_names, hb_char_t **arg_vals) {
  // TODO
}

static void _hbs_dir_entity(hbs_options_t so, hbu_pipe_t pipe) {
  // TODO
}

static void _hbs_dir_tag(hbs_options_t so, hbu_pipe_t pipe) {
  // TODO
}

void hbs_dir(hbs_options_t so, hbu_pipe_t pipe) {
  hb_char_t c = hbu_pipe_peek(pipe);

  switch (c) {
  case '&':
    _hbs_dir_entity(so, pipe);
    break;

  case '<':
    _hbs_dir_tag(so, pipe);
    break;

  default:
    hbe_fatal(HBE_INTERR_NOT_A_HB_DIR, "INTERR Next character is not the start of a directive tag or entity");
  }
}

#endif // _HDR_HYPERBUILD_STREAM_DIR
