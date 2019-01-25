static void _hbs_script_slcomment(hbe_err_t *hbe_err, hb_proc_t pipe) {
  HBE_CATCH_V(hb_proc_require_match, pipe, "//");

  // Comment can end at closing </script>
  // NOTE: Closing tag must not contain whitespace
  while (1) {
    int line_term = HBE_CATCH_V(hb_proc_accept_if_matches_line_terminator, pipe);
    if (line_term) {
      break;
    }

    int end_tag = HBE_CATCH_V(hb_proc_matches_i, pipe, "</script>");
    if (end_tag) {
      break;
    }

    HBE_CATCH_V(hb_proc_accept, pipe);
  }
}

static void _hbs_script_mlcomment(hbe_err_t *hbe_err, hb_proc_t pipe) {
  HBE_CATCH_V(hb_proc_require_match, pipe, "/*");

  // Comment can end at closing </script>
  // NOTE: Closing tag must not contain whitespace
  while (1) {
    int end = HBE_CATCH_V(hb_proc_accept_if_matches, pipe, "*/");
    if (end) {
      break;
    }

    int end_tag = HBE_CATCH_V(hb_proc_matches_i, pipe, "</script>");
    if (end_tag) {
      break;
    }

    HBE_CATCH_V(hb_proc_accept, pipe);
  }
}

static void _hbs_script_string(hbe_err_t *hbe_err, hb_proc_t pipe) {
  hb_proc_char_t delim = HBE_CATCH_V(hb_proc_accept, pipe);

  if (delim != '"' && delim != '\'') {
    hb_proc_THROW_V(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected JavaScript string delimiter");
  }

  int escaping = 0;

  while (1) {
    hb_proc_char_t c = HBE_CATCH_V(hb_proc_accept, pipe);

    if (c == '\\') {
      escaping ^= 1;
      continue;
    }

    if (c == delim && !escaping) {
      break;
    }

    int line_term = HBE_CATCH_V(hb_proc_accept_if_matches_line_terminator, pipe);
    if (line_term) {
      if (!escaping) {
        hb_proc_THROW_V(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Unterminated JavaScript string");
      }
    }

    escaping = 0;
  }
}

static void _hbs_script_template(hbe_err_t *hbe_err, hb_proc_t pipe) {
  HBE_CATCH_V(hb_proc_require_match, pipe, "`");

  int escaping = 0;

  while (1) {
    hb_proc_char_t c = HBE_CATCH_V(hb_proc_accept, pipe);

    if (c == '\\') {
      escaping ^= 1;
      continue;
    }

    if (c == '`' && !escaping) {
      break;
    }

    escaping = 0;
  }
}

void hbs_script(hbe_err_t *hbe_err, hb_proc_t pipe) {
  while (1) {
    int end = HBE_CATCH_V(hb_proc_matches, pipe, "</");
    if (end) {
      break;
    }

    int sl_comment = HBE_CATCH_V(hb_proc_matches, pipe, "//");
    if (sl_comment) {
      HBE_CATCH_V(_hbs_script_slcomment, pipe);
      continue;
    }

    int ml_comment = HBE_CATCH_V(hb_proc_matches, pipe, "/*");
    if (ml_comment) {
      HBE_CATCH_V(_hbs_script_mlcomment, pipe);
      continue;
    }

    hb_proc_char_t next = HBE_CATCH_V(hb_proc_peek, pipe);
    if (next == '"' || next == '\'') {
      HBE_CATCH_V(_hbs_script_string, pipe);
      continue;
    }

    if (next == '`') {
      HBE_CATCH_V(_hbs_script_template, pipe);
      continue;
    }

    HBE_CATCH_V(hb_proc_accept, pipe);
  }
}
