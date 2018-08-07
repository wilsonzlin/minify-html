#ifndef _HDR_HYPERBUILD_STREAM_BANG
#define _HDR_HYPERBUILD_STREAM_BANG

#include "../util/hbchar.h"
#include "../util/pipe.c"

void hbs_bang(hbu_pipe_t pipe)
{
  hbu_pipe_require_match(pipe, "<!");

  while (1)
  {
    if (hbu_pipe_peek_offset(pipe, 1) == '>')
    {
      break;
    }

    hbu_pipe_accept(pipe);
  }

  hbu_pipe_accept(pipe);
}

#endif // _HDR_HYPERBUILD_STREAM_BANG
