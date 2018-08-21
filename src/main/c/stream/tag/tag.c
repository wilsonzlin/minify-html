// Declare first before content.c, as content.c depends on it
void hbs_tag(hbe_err_t *hbe_err, hbu_streamoptions_t so, hbu_pipe_t pipe, hb_char_t *parent);

#include "./tagname.c"
#include "../attr/attr.c"
#include "../content/script.c"
#include "../content/style.c"
#include "../content/html.c"

// $parent could be NULL
void hbs_tag(hbe_err_t *hbe_err, hbu_streamoptions_t so, hbu_pipe_t pipe, hb_char_t *parent) {
  hbu_list_char_t opening_name = NULL;

  int self_closing = 0;

  HBE_CATCH_F(hbu_pipe_require, pipe, '<');
  opening_name = HBE_CATCH_F(hbs_tagname, so, pipe);

  int last_attr_type = -1;

  while (1) {
    // At the beginning of this loop, the last parsed unit was either the tag name
    // or an attribute (including its value, if it had one)
    size_t ws_accepted;
    if (so->remove_tag_whitespace) {
      ws_accepted = HBE_CATCH_F(hbu_pipe_skip_while_predicate, pipe, &hbr_whitespace_check);
    } else {
      ws_accepted = HBE_CATCH_F(hbu_pipe_accept_while_predicate, pipe, &hbr_whitespace_check);
    }

    int end_of_tag = HBE_CATCH_F(hbu_pipe_accept_if, pipe, '>');
    if (end_of_tag) {
      break;
    }

    self_closing = HBE_CATCH_F(hbu_pipe_accept_if_matches, pipe, "/>");
    if (self_closing) {
      if (!hbu_streamoptions_supressed_error(so, HBE_PARSE_SELF_CLOSING_TAG)) {
        HBU_PIPE_THROW_F(pipe, HBE_PARSE_SELF_CLOSING_TAG, "Self-closing tag");
      }
      break;
    }

    // HBE_PARSE_NO_SPACE_BEFORE_ATTR is not suppressable as then there would be difficulty
    // in determining what is the end of a tag/attribute name/attribute value
    if (!ws_accepted) {
      HBU_PIPE_THROW_F(pipe, HBE_PARSE_NO_SPACE_BEFORE_ATTR, "No whitespace before attribute");
    }

    if (so->remove_tag_whitespace) {
      if (last_attr_type != HBS_ATTR_QUOTED) {
        HBE_CATCH_F(hbu_pipe_write, pipe, ' ');
      }
    }

    last_attr_type = HBE_CATCH_F(hbs_attr, so, pipe);
  }

  hb_char_t *tag_name = hbu_list_char_underlying(opening_name);

  // Non-standard tag checking is done in hbs_tagname
  if (parent != NULL && (
      !hbr_whitelistparents_allowed(tag_name, parent) ||
      !hbr_whitelistchildren_allowed(parent, tag_name) ||
      !hbr_blacklistparents_allowed(tag_name, parent) ||
      !hbr_blacklistchildren_allowed(parent, tag_name))) {
    HBU_PIPE_THROW_F(pipe, HBE_PARSE_ILLEGAL_CHILD, "Tag can't be a child there");
  }

  // Self-closing or void tag
  if (self_closing || hbr_voidtags_check(tag_name)) {
    goto finally;
  }

  if (hbu_list_char_compare_lit(opening_name, "script") == 0) {
    // Script tag
    HBE_CATCH_F(hbs_script, pipe);
  } else if (hbu_list_char_compare_lit(opening_name, "style") == 0) {
    // Style tag
    HBE_CATCH_F(hbs_style, pipe);
  } else {
    // Content
    HBE_CATCH_F(hbs_content, so, pipe, tag_name);
  }

  // Closing tag for non-void
  HBE_CATCH_F(hbu_pipe_require, pipe, '<');
  HBE_CATCH_F(hbu_pipe_require, pipe, '/');
  hbu_list_char_t closing_name = HBE_CATCH_F(hbs_tagname, so, pipe);
  HBE_CATCH_F(hbu_pipe_require, pipe, '>');

  if (!hbu_list_char_equal(opening_name, closing_name)) {
    HBU_PIPE_THROW_F(pipe, HBE_PARSE_UNCLOSED_TAG, "Tag not closed (expected `%s` closing tag, got `%s`)", tag_name, hbu_list_char_underlying(closing_name));
  }

  finally:
    if (opening_name) {
      hbu_list_char_destroy(opening_name);
      opening_name = NULL;
    }
    return;
}
