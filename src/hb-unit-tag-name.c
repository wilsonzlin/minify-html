hb_list_char_t hbs_tagname(hbe_err_t *hbe_err, hbu_streamoptions_t so, hb_proc_t pipe) {
  hb_list_char_t name = hb_list_char_create();

  while (1) {
    hb_proc_char_t c = HBE_CATCH_F(hb_proc_peek, pipe);

    if (!hb_rule_tagname_check(c)) {
      break;
    }

    if (hb_rule_ucalpha_check(c)) {
      if (!hbu_streamoptions_supressed_error(so, HBE_PARSE_UCASE_TAG)) {
        hb_proc_THROW_F(pipe, HBE_PARSE_UCASE_TAG, "Uppercase character in tag");
      }
      // Lowercase to normalise when checking against rules and closing tag
      hb_list_char_append(name, c + 32);
    } else {
      hb_list_char_append(name, c);
    }

    HBE_CATCH_F(hb_proc_accept, pipe);
  }

  if (!hbu_streamoptions_supressed_error(so, HBE_PARSE_NONSTANDARD_TAG) && !hb_rule_tags_check(hb_list_char_underlying(name))) {
    hb_proc_THROW_F(pipe, HBE_PARSE_NONSTANDARD_TAG, "Non-standard tag");
  }

  finally:
    if (*hbe_err != NULL) {
      hb_list_char_destroy(name);
      name = NULL;
    }
    return name;
}
