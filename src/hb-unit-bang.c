void hbs_bang(hbe_err_t *hbe_err, hb_proc_t pipe) {
  HBE_CATCH_V(hb_proc_require_match, pipe, "<!");

  while (1) {
    hb_proc_char_t next = HBE_CATCH_V(hb_proc_peek, pipe);
    if (next == '>') {
      break;
    }

    HBE_CATCH_V(hb_proc_accept, pipe);
  }

  HBE_CATCH_V(hb_proc_accept, pipe);
}
