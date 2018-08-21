hbu_list_char_t hbs_tagname(hbe_err_t *hbe_err, hbu_streamoptions_t so, hbu_pipe_t pipe) {
  hbu_list_char_t name = hbu_list_char_create();

  while (1) {
    hb_char_t c = HBE_CATCH_F(hbu_pipe_peek, pipe);

    if (!hbr_tagname_check(c)) {
      break;
    }

    if (hbr_ucalpha_check(c)) {
      if (!hbu_streamoptions_supressed_error(so, HBE_PARSE_UCASE_TAG)) {
        HBU_PIPE_THROW_F(pipe, HBE_PARSE_UCASE_TAG, "Uppercase character in tag");
      }
      // Lowercase to normalise when checking against rules and closing tag
      hbu_list_char_append(name, c + 32);
    } else {
      hbu_list_char_append(name, c);
    }

    HBE_CATCH_F(hbu_pipe_accept, pipe);
  }

  if (!hbu_streamoptions_supressed_error(so, HBE_PARSE_NONSTANDARD_TAG) && !hbr_tags_check(hbu_list_char_underlying(name))) {
    HBU_PIPE_THROW_F(pipe, HBE_PARSE_NONSTANDARD_TAG, "Non-standard tag");
  }

  finally:
    if (*hbe_err != NULL) {
      hbu_list_char_destroy(name);
      name = NULL;
    }
    return name;
}
