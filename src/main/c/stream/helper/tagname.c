#ifndef _HDR_HYPERBUILD_STREAM_HELPER_TAGNAME
#define _HDR_HYPERBUILD_STREAM_HELPER_TAGNAME

#include "../../rule/char/tagname.c"

#include "../../util/hbchar.h"
#include "../../util/pipe.c"

hbu_buffer_t hbsh_tagname(hbu_pipe_t pipe) {
  hbu_buffer_t name = hbu_buffer_create();

  while (1) {
    hb_char_t c = hbu_pipe_peek(pipe);

    if (!hbr_tagname_check(c)) {
      break;
    }

    hbu_buffer_append(name, c);

    hbu_pipe_accept(pipe);
  }

  return name;
}

#endif // _HDR_HYPERBUILD_STREAM_HELPER_TAGNAME
