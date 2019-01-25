#include "./attrval.c"
#include "./quoteattrval.c"
#include "./unquoteattrval.c"

hbs_attr_val_type_t hbs_attr(hbe_err_t *hbe_err, hbu_streamoptions_t so, hb_proc_t pipe) {
  hbs_attr_val_type_t rv = 0;
  hb_list_char_t name = hb_list_char_create();

  while (1) {
    // Char matched by $attrname required at least once
    hb_proc_char_t c = HBE_CATCH_F(hb_proc_require_predicate, pipe, &hb_rule_attrname_check, "attribute name");

    if (hb_rule_ucalpha_check(c)) {
      if (!hbu_streamoptions_supressed_error(so, HBE_PARSE_UCASE_ATTR)) {
        hb_proc_THROW_F(pipe, HBE_PARSE_UCASE_ATTR, "Uppercase letter in attribute name");
      }
      // Lowercase to normalise when checking against rules and closing tag
      hb_list_char_append(name, c + 32);
    } else {
      hb_list_char_append(name, c);
    }

    // Don't use hb_proc_accept_while_predicate as advanced checks might be needed
    hb_proc_char_t n = HBE_CATCH_F(hb_proc_peek, pipe);

    if (!hb_rule_attrname_check(n)) {
      break;
    }
  }

  int collapse_and_trim_whitespace = hb_list_char_compare_lit(name, "class") == 0 &&
                                     so->trim_class_attr;

  int has_value = HBE_CATCH_F(hb_proc_accept_if, pipe, '=');
  if (has_value) {
    hb_proc_char_t next = HBE_CATCH_F(hb_proc_peek, pipe);
    if (hb_rule_attrvalquote_check(next)) {
      // Quoted attribute value
      hbs_attr_val_type_t type = HBE_CATCH_F(hbs_quoteattrval, so, pipe, collapse_and_trim_whitespace);
      // Don't inline as HBE_CATCH_F adds auxiliary statements
      HB_RETURN_F(type);
    }

    if (!hbu_streamoptions_supressed_error(so, HBE_PARSE_UNQUOTED_ATTR)) {
      hb_proc_THROW_F(pipe, HBE_PARSE_UNQUOTED_ATTR, "Unquoted attribute value");
    }
    // Unquoted attribute value
    HBE_CATCH_F(hbs_unquoteattrval, so, pipe);
    HB_RETURN_F(HBS_ATTR_UNQUOTED);
  }

  finally:
    hb_list_char_destroy(name);
    name = NULL;
    return rv;
}
