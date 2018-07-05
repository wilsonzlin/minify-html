#ifndef _HDR_HYPERBUILD_STREAM_HELPER_QUOTEATTRVAL
#define _HDR_HYPERBUILD_STREAM_HELPER_QUOTEATTRVAL

#include "../../rule/char/attrvalquote.c"

#include "../../util/hbchar.h"
#include "../../util/pipe.c"

void hbsh_quoteattrval(hbu_pipe_t pipe) {
  hb_char_t quote_char = hbu_pipe_require_predicate(pipe, &hb_r_attrvalquote_check, "attribute value quote");

  while (hbu_pipe_peek(pipe) != quote_char) {
    hbu_pipe_accept(pipe);
  }

  hbu_pipe_require(pipe, quote_char);
}

#endif // _HDR_HYPERBUILD_STREAM_HELPER_QUOTEATTRVAL
