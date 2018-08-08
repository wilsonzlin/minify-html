#ifndef _HDR_HYPERBUILD_STREAM_TAG
#define _HDR_HYPERBUILD_STREAM_TAG

#include "../error/error.c"

#include "../rule/char/whitespace.c"
#include "../rule/tag/voidtags.c"
#include "../rule/relation/blacklistchildren.c"
#include "../rule/relation/blacklistparents.c"
#include "../rule/relation/whitelistchildren.c"
#include "../rule/relation/whitelistparents.c"

#include "../util/hbchar.h"
#include "../util/pipe.c"

#include "./helper/tagname.c"
#include "./helper/attr.c"
#include "./helper/script.c"
#include "./helper/style.c"

// Declare first before content.c, as content.c depends on it
void hbs_tag(hbs_options_t so, hbu_pipe_t pipe, hb_char_t *parent);

#include "./streamoptions.c"
#include "./content.c"

// $parent could be NULL
void hbs_tag(hbs_options_t so, hbu_pipe_t pipe, hb_char_t *parent) {
  int self_closing = 0;

  hbu_pipe_require(pipe, '<');
  hbu_buffer_t opening_name = hbsh_tagname(so, pipe);

  int last_attr_type = -1;

  while (1) {
    // At the beginning of this loop, the last parsed unit was either the tag name
    // or an attribute (including its value, if it had one)
    size_t ws_accepted;
    if (so->remove_tag_whitespace) {
      ws_accepted = hbu_pipe_skip_while_predicate(pipe, &hbr_whitespace_check);
    } else {
      ws_accepted = hbu_pipe_accept_while_predicate(pipe, &hbr_whitespace_check);
    }

    if (hbu_pipe_accept_if(pipe, '>')) {
      break;
    }

    if (hbu_pipe_accept_if_matches(pipe, "/>")) {
      if (!hbs_options_supressed_error(so, HBE_PARSE_SELF_CLOSING_TAG)) {
        hbu_pipe_error(pipe, HBE_PARSE_SELF_CLOSING_TAG, "Self-closing tag");
      }
      self_closing = 1;
      break;
    }

    if (!ws_accepted) {
      hbu_pipe_error(pipe, HBE_PARSE_NO_SPACE_BEFORE_ATTR, "No whitespace before attribute");
    }

    if (so->remove_tag_whitespace) {
      if (last_attr_type != HBSH_ATTR_QUOTED) {
        hbu_pipe_write(pipe, ' ');
      }
    }

    last_attr_type = hbsh_attr(so, pipe);
  }

  hb_char_t *tag_name = hbu_buffer_underlying(opening_name);

  // Non-standard tag checking is done in hbsh_tagname
  if (parent != NULL && (
      !hbr_whitelistparents_allowed(tag_name, parent) ||
      !hbr_whitelistchildren_allowed(parent, tag_name) ||
      !hbr_blacklistparents_allowed(tag_name, parent) ||
      !hbr_blacklistchildren_allowed(parent, tag_name))) {
    hbu_pipe_error(pipe, HBE_PARSE_ILLEGAL_CHILD, "Tag can't be a child there");
  }

  // Self-closing or void tag
  if (self_closing || hbr_voidtags_check(tag_name)) {
    goto cleanup;
  }

  if (hbu_buffer_compare_lit(opening_name, "script") == 0) {
    // Script tag
    hbsh_script(pipe);
  } else if (hbu_buffer_compare_lit(opening_name, "style") == 0) {
    // Style tag
    hbsh_style(pipe);
  } else {
    // Content
    hbs_content(so, pipe, tag_name);
  }

  // Closing tag for non-void
  hbu_pipe_require(pipe, '<');
  hbu_pipe_require(pipe, '/');
  hbu_buffer_t closing_name = hbsh_tagname(so, pipe);
  hbu_pipe_require(pipe, '>');

  if (!hbu_buffer_equal(opening_name, closing_name)) {
    hbu_pipe_error(pipe, HBE_PARSE_UNCLOSED_TAG, "Tag not closed (expected `%s` closing tag, got `%s`)", tag_name, hbu_buffer_underlying(closing_name));
  }

  goto cleanup;

  cleanup:
    hbu_buffer_destroy(opening_name);
    opening_name = NULL;
    return;
}

#endif // _HDR_HYPERBUILD_STREAM_TAG
