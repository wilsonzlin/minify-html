void hbs_comment(hbe_err_t *hbe_err, hbu_streamoptions_t so, hbu_pipe_t pipe) {
  hbu_pipe_toggle_output_mask(pipe, so->remove_comments);

  HBE_CATCH_V(hbu_pipe_require_match, pipe, "<!--");

  while (1) {
    int end = HBE_CATCH_V(hbu_pipe_accept_if_matches, pipe, "-->");
    if (end) {
      break;
    }

    HBE_CATCH_V(hbu_pipe_accept, pipe);
  }

  hbu_pipe_toggle_output_mask(pipe, 0);
}
