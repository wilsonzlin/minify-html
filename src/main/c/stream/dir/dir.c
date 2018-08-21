static void _hbs_dir_eval(hbu_streamoptions_t so, hbu_pipe_t pipe, hb_char_t *name, hb_char_t **arg_names, hb_char_t **arg_vals) {
  // TODO
}

static void _hbs_dir_entity(hbu_streamoptions_t so, hbu_pipe_t pipe) {
  // TODO
}

static void _hbs_dir_tag(hbu_streamoptions_t so, hbu_pipe_t pipe) {
  // TODO
}

void hbs_dir(hbu_streamoptions_t so, hbu_pipe_t pipe) {
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
