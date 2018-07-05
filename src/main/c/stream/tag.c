#ifndef _HDR_HYPERBUILD_STREAM_TAG
#define _HDR_HYPERBUILD_STREAM_TAG

#include "../rule/tag/voidtags.c"

#include "../util/hbchar.h"
#include "../util/pipe.c"

#include "./helper/tagname.c"
// Declare first before content.c, as content.c depends on it
void hbs_tag(hbu_pipe_t pipe);

#include "./content.c"

void hbs_tag(hbu_pipe_t pipe) {
  hbu_pipe_require(pipe, '<');
  hbu_buffer_t opening_name = hbsh_tagname(pipe);
  // TODO
  hbu_pipe_require(pipe, '>');

  // Content
  hbs_content(pipe);

  // Closing tag for non-void
  hbu_pipe_require(pipe, '<');
  hbu_pipe_require(pipe, '/');
  hbu_buffer_t closing_name = hbsh_tagname(pipe);
  hbu_pipe_require(pipe, '>');
}

#endif // _HDR_HYPERBUILD_STREAM_TAG
