void hbs_comment(hbe_err_t *hbe_err, hbu_streamoptions_t so, hb_proc_t pipe) {
  hb_proc_toggle_output_mask(pipe, so->remove_comments);

  HBE_CATCH_V(hb_proc_require_match, pipe, "<!--");

  while (1) {
    int end = HBE_CATCH_V(hb_proc_accept_if_matches, pipe, "-->");
    if (end) {
      break;
    }

    HBE_CATCH_V(hb_proc_accept, pipe);
  }

  hb_proc_toggle_output_mask(pipe, 0);
}
