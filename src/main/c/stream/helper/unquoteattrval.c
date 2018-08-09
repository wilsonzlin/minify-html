#ifndef _HDR_HYPERBUILD_STREAM_HELPER_UNQUOTEATTRVAL
#define _HDR_HYPERBUILD_STREAM_HELPER_UNQUOTEATTRVAL

#include "../../rule/char/unquotedattrval.c"

#include "../../util/pipe.c"

#include "./entity.c"

void hbsh_unquoteattrval(hbs_options_t so, hbu_pipe_t pipe) {
  // Can't use hbu_pipe_require_predicate, as first char might be `&`,
  // which needs to be processed by hbsh_entity
  int at_least_one_char = 0;

  hb_char_t c;
  while (1) {
    c = hbu_pipe_peek(pipe);

    if (!hbr_unquotedattrval_check(c)) {
      return;
    }

    at_least_one_char = 1;

    if (c == '&') {
      hbsh_entity(so, pipe);
    } else {
      hbu_pipe_accept(pipe);
    }
  }

  if (!at_least_one_char) {
    hbu_pipe_error(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected unquoted attribute value, got `%c` (0x%x)", c);
  }
}

#endif // _HDR_HYPERBUILD_STREAM_HELPER_UNQUOTEATTRVAL
