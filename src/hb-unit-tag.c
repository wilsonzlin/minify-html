// Declare first before content.c, as content.c depends on it
void hbs_tag(hbe_err_t *hbe_err, hbu_streamoptions_t so, hb_proc_t pipe, hb_proc_char_t *parent);

#include "./tagname.c"
#include "../attr/attr.c"
#include "../content/script.c"
#include "../content/style.c"
#include "../content/html.c"

// $parent could be NULL
void hbs_tag(hbe_err_t *hbe_err, hbu_streamoptions_t so, hb_proc_t pipe, hb_proc_char_t *parent) {
  hb_list_char_t opening_name = NULL;

  int self_closing = 0;

  HBE_CATCH_F(hb_proc_require, pipe, '<');
  opening_name = HBE_CATCH_F(hbs_tagname, so, pipe);

  int last_attr_type = -1;

  while (1) {
    // At the beginning of this loop, the last parsed unit was either the tag name
    // or an attribute (including its value, if it had one)
    size_t ws_accepted;
    if (so->remove_tag_whitespace) {
      ws_accepted = HBE_CATCH_F(hb_proc_skip_while_predicate, pipe, &hb_rule_whitespace_check);
    } else {
      ws_accepted = HBE_CATCH_F(hb_proc_accept_while_predicate, pipe, &hb_rule_whitespace_check);
    }

    int end_of_tag = HBE_CATCH_F(hb_proc_accept_if, pipe, '>');
    if (end_of_tag) {
      break;
    }

    self_closing = HBE_CATCH_F(hb_proc_accept_if_matches, pipe, "/>");
    if (self_closing) {
      if (!hbu_streamoptions_supressed_error(so, HBE_PARSE_SELF_CLOSING_TAG)) {
        hb_proc_THROW_F(pipe, HBE_PARSE_SELF_CLOSING_TAG, "Self-closing tag");
      }
      break;
    }

    // HBE_PARSE_NO_SPACE_BEFORE_ATTR is not suppressable as then there would be difficulty
    // in determining what is the end of a tag/attribute name/attribute value
    if (!ws_accepted) {
      hb_proc_THROW_F(pipe, HBE_PARSE_NO_SPACE_BEFORE_ATTR, "No whitespace before attribute");
    }

    if (so->remove_tag_whitespace) {
      if (last_attr_type != HBS_ATTR_QUOTED) {
        HBE_CATCH_F(hb_proc_write, pipe, ' ');
      }
    }

    last_attr_type = HBE_CATCH_F(hbs_attr, so, pipe);
  }

  hb_proc_char_t *tag_name = hb_list_char_underlying(opening_name);

  // Non-standard tag checking is done in hbs_tagname
  if (parent != NULL && (
      !hb_rule_whitelistparents_allowed(tag_name, parent) ||
      !hb_rule_whitelistchildren_allowed(parent, tag_name) ||
      !hb_rule_blacklistparents_allowed(tag_name, parent) ||
      !hb_rule_blacklistchildren_allowed(parent, tag_name))) {
    hb_proc_THROW_F(pipe, HBE_PARSE_ILLEGAL_CHILD, "Tag can't be a child there");
  }

  // Self-closing or void tag
  if (self_closing || hb_rule_voidtags_check(tag_name)) {
    goto finally;
  }

  if (hb_list_char_compare_lit(opening_name, "script") == 0) {
    // Script tag
    HBE_CATCH_F(hbs_script, pipe);
  } else if (hb_list_char_compare_lit(opening_name, "style") == 0) {
    // Style tag
    HBE_CATCH_F(hbs_style, pipe);
  } else {
    // Content
    HBE_CATCH_F(hbs_content, so, pipe, tag_name);
  }

  // Closing tag for non-void
  HBE_CATCH_F(hb_proc_require, pipe, '<');
  HBE_CATCH_F(hb_proc_require, pipe, '/');
  hb_list_char_t closing_name = HBE_CATCH_F(hbs_tagname, so, pipe);
  HBE_CATCH_F(hb_proc_require, pipe, '>');

  if (!hb_list_char_equal(opening_name, closing_name)) {
    hb_proc_THROW_F(pipe, HBE_PARSE_UNCLOSED_TAG, "Tag not closed (expected `%s` closing tag, got `%s`)", tag_name, hb_list_char_underlying(closing_name));
  }

  finally:
    if (opening_name) {
      hb_list_char_destroy(opening_name);
      opening_name = NULL;
    }
    return;
}
