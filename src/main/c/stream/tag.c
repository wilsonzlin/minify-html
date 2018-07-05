#ifndef _HDR_HYPERBUILD_STREAM_TAG
#define _HDR_HYPERBUILD_STREAM_TAG

#include "../error/error.c"

#include "../rule/char/whitespace.c"
#include "../rule/tag/voidtags.c"

#include "../util/hbchar.h"
#include "../util/pipe.c"

#include "./helper/tagname.c"
#include "./helper/attr.c"

// Declare first before content.c, as content.c depends on it
void hbs_tag(hbu_pipe_t pipe);

#include "./content.c"

void hbs_tag(hbu_pipe_t pipe) {
  hbu_pipe_require(pipe, '<');
  hbu_buffer_t opening_name = hbsh_tagname(pipe);
  while (hbu_pipe_peek(pipe) != '>') {
    hbu_pipe_require_predicate(pipe, &hb_r_whitespace_check, "whitespace between attributes");
    hbu_pipe_skip_while_predicate(pipe, &hb_r_whitespace_check);

    hbsh_attr(pipe);
  }
  hbu_pipe_require(pipe, '>');

  if (hb_r_voidtags_check(hbu_buffer_underlying(opening_name))) {
    return;
  }

  // Content
  hbs_content(pipe);

  // Closing tag for non-void
  hbu_pipe_require(pipe, '<');
  hbu_pipe_require(pipe, '/');
  hbu_buffer_t closing_name = hbsh_tagname(pipe);
  hbu_pipe_require(pipe, '>');

  if (!hbu_buffer_equal(opening_name, closing_name)) {
    hbe_fatal(HBE_PARSE_UNCLOSED_TAG, "Tag not closed");
  }
}

#endif // _HDR_HYPERBUILD_STREAM_TAG
