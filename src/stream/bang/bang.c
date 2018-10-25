void hbs_bang(hbe_err_t *hbe_err, hbu_pipe_t pipe) {
  HBE_CATCH_V(hbu_pipe_require_match, pipe, "<!");

  while (1) {
    hb_char_t next = HBE_CATCH_V(hbu_pipe_peek, pipe);
    if (next == '>') {
      break;
    }

    HBE_CATCH_V(hbu_pipe_accept, pipe);
  }

  HBE_CATCH_V(hbu_pipe_accept, pipe);
}
