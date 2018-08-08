#ifndef _HDR_HYPERBUILD_STREAM_HELPER_QUOTEATTRVAL
#define _HDR_HYPERBUILD_STREAM_HELPER_QUOTEATTRVAL

#include "../../rule/char/attrvalquote.c"

#include "../../util/hbchar.h"
#include "../../util/pipe.c"

void hbsh_quoteattrval(hbu_pipe_t pipe, int collapse_and_trim_whitespace) {
  hb_char_t quote_char = hbu_pipe_require_predicate(pipe, &hbr_attrvalquote_check, "attribute value quote");

  int whitespace = 0;
  if (collapse_and_trim_whitespace) {
    hbu_pipe_skip_while_predicate(pipe, &hbr_whitespace_check);
  }

  hb_char_t c;

  while ((c = hbu_pipe_peek(pipe)) != quote_char) {
    if (collapse_and_trim_whitespace && hbr_whitespace_check(c)) {
      whitespace = 1;
      hbu_pipe_skip(pipe);
    } else {
      if (whitespace) {
        whitespace = 0;
        hbu_pipe_write(pipe, ' ');
      }
      hbu_pipe_accept(pipe);
    }
  }

  hbu_pipe_require(pipe, quote_char);
}

#endif // _HDR_HYPERBUILD_STREAM_HELPER_QUOTEATTRVAL
