static void _hbs_style_comment(hbe_err_t *hbe_err, hb_proc_t pipe) {
  HBE_CATCH_V(hb_proc_require_match, pipe, "/*");

  // Unlike script tags, style comments do NOT end at closing tag
  while (1) {
    int is_end = HBE_CATCH_V(hb_proc_accept_if_matches, pipe, "*/");
    if (is_end) {
      break;
    }
    HBE_CATCH_V(hb_proc_accept, pipe);
  }
}

static void _hbs_style_string(hbe_err_t *hbe_err, hb_proc_t pipe) {
  hb_proc_char_t delim = HBE_CATCH_V(hb_proc_accept, pipe);

  if (delim != '"' && delim != '\'') {
    hb_proc_THROW_V(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected CSS string delimiter");
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
        hb_proc_THROW_V(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Unterminated CSS string");
      }
    }

    escaping = 0;
  }
}

void hbs_style(hbe_err_t *hbe_err, hb_proc_t pipe) {
  while (1) {
    int end = HBE_CATCH_V(hb_proc_matches, pipe, "</");
    if (end) {
      break;
    }

    int is_comment = HBE_CATCH_V(hb_proc_matches, pipe, "/*");
    if (is_comment) {
      HBE_CATCH_V(_hbs_style_comment, pipe);
      continue;
    }

    hb_proc_char_t next = HBE_CATCH_V(hb_proc_peek, pipe);
    if (next == '"' || next == '\'') {
      HBE_CATCH_V(_hbs_style_string, pipe);
      continue;
    }

    HBE_CATCH_V(hb_proc_accept, pipe);
  }
}
