#ifndef _HDR_HYPERBUILD_STREAM_COMMENT
#define _HDR_HYPERBUILD_STREAM_COMMENT

#include "../util/hbchar.h"
#include "../util/pipe.c"

void hbs_comment(hbu_pipe_t pipe)
{
  hbu_pipe_require(pipe, '<');
  hbu_pipe_require(pipe, '!');
  hbu_pipe_require(pipe, '-');
  hbu_pipe_require(pipe, '-');

  while (1)
  {
    if (hbu_pipe_peek_offset(pipe, 1) == '-' &&
        hbu_pipe_peek_offset(pipe, 2) == '-' &&
        hbu_pipe_peek_offset(pipe, 3) == '>')
    {
      break;
    }

    hbu_pipe_skip(pipe);
  }

  hbu_pipe_skip_amount(pipe, 3);
}

#endif // _HDR_HYPERBUILD_STREAM_COMMENT
