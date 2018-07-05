#ifndef _HDR_HYPERBUILD_STREAM_CONTENT
#define _HDR_HYPERBUILD_STREAM_CONTENT

#include "../util/hbchar.h"
#include "../util/pipe.c"

// Declare first before tag.c, as tag.c depends on it
void hbs_content(hbu_pipe_t pipe);

#include "./tag.c"

void hbs_content(hbu_pipe_t pipe) {
  while (1) {
    hb_eod_char_t c = hbu_pipe_peek_eoi(pipe);

    switch (c) {
    case HB_EOD:
      return;

    case '<':
      if (hbu_pipe_peek_offset(pipe, 2) == '/') {
        // Callee is responsible for requiring close tag (or not, if root)
        return;
      }

      hbs_tag(pipe);
      break;

    case '&':
      // TODO
      break;

    default:
      hbu_pipe_accept(pipe);
    }
  }
}

#endif // _HDR_HYPERBUILD_STREAM_CONTENT
