#include "./attrval.c"
#include "../entity/entity.c"

hbs_attr_val_type_t hbs_quoteattrval(hbe_err_t *hbe_err, hbu_streamoptions_t so, hb_proc_t pipe, int collapse_and_trim_whitespace) {
  hbs_attr_val_type_t rv = 0;
  int should_remove_attr_quotes = so->remove_attr_quotes;

  hb_list_char_t value = NULL;
  if (should_remove_attr_quotes) {
    value = hb_list_char_create();
    hb_proc_set_redirect(pipe, value);
  }

  hb_proc_char_t quote_char = HBE_CATCH_F(hb_proc_require_skip_predicate, pipe, &hb_rule_attrvalquote_check, "attribute value quote");
  if (!should_remove_attr_quotes) {
    // Not buffering, so directly output
    HBE_CATCH_F(hb_proc_write, pipe, quote_char);
  }

  int whitespace = 0;
  if (collapse_and_trim_whitespace) {
    HBE_CATCH_F(hb_proc_skip_while_predicate, pipe, &hb_rule_whitespace_check);
  }

  int not_empty = 0;
  int can_be_unquoted = 1;

  while (1) {
    hb_proc_char_t c = HBE_CATCH_F(hb_proc_peek, pipe);
    if (c == quote_char) {
      break;
    }

    not_empty = 1;
    can_be_unquoted = can_be_unquoted && hb_rule_unquotedattrval_check(c);

    if (collapse_and_trim_whitespace && hb_rule_whitespace_check(c)) {
      whitespace = 1;
      HBE_CATCH_F(hb_proc_skip, pipe);

    } else {
      if (whitespace) {
        whitespace = 0;
        HBE_CATCH_F(hb_proc_write, pipe, ' ');
      }

      if (c == '&') {
        // Call hbs_entity even if not decoding entities, as it will validate the entity
        HBE_CATCH_F(hbs_entity, so, pipe);
      } else {
        HBE_CATCH_F(hb_proc_accept, pipe);
      }
    }
  }

  can_be_unquoted = not_empty && can_be_unquoted;

  HBE_CATCH_F(hb_proc_require_skip, pipe, quote_char);
  if (!should_remove_attr_quotes) {
    // Not buffering, so directly output
    HBE_CATCH_F(hb_proc_write, pipe, quote_char);
    HB_RETURN_F(HBS_ATTR_QUOTED);

  } else {
    hb_proc_set_redirect(pipe, NULL);
    if (!can_be_unquoted) {
      // Wrap in braces as macro will add auxiliary statement
      HBE_CATCH_F(hb_proc_write, pipe, quote_char);
    }
    HBE_CATCH_F(hb_proc_write_buffer, pipe, value);
    if (!can_be_unquoted) {
      // Wrap in braces as macro will add auxiliary statement
      HBE_CATCH_F(hb_proc_write, pipe, quote_char);
    }
    HB_RETURN_F(can_be_unquoted ? HBS_ATTR_UNQUOTED : HBS_ATTR_QUOTED);
  }

  finally:
    if (value != NULL) {
      hb_list_char_destroy(value);
      value = NULL;
    }
    return rv;
}
