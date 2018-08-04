#ifndef _HDR_HYPERBUILD_STREAM_COMMENT
#define _HDR_HYPERBUILD_STREAM_COMMENT

#include "../util/hbchar.h"
#include "../util/pipe.c"

#include "./streamoptions.c"

void hbs_comment(hbs_options_t so, hbu_pipe_t pipe)
{
  hbu_pipe_toggle_output_mask(pipe, so->remove_comments);

  hbu_pipe_require_match(pipe, "<!--");

  while (1)
  {
    if (hbu_pipe_accept_if_matches(pipe, "-->"))
    {
      break;
    }

    hbu_pipe_accept(pipe);
  }

  hbu_pipe_toggle_output_mask(pipe, 0);
}

#endif // _HDR_HYPERBUILD_STREAM_COMMENT
