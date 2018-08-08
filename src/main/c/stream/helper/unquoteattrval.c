#ifndef _HDR_HYPERBUILD_STREAM_HELPER_UNQUOTEATTRVAL
#define _HDR_HYPERBUILD_STREAM_HELPER_UNQUOTEATTRVAL

#include "../../rule/char/unquotedattrval.c"

#include "../../util/pipe.c"

void hbsh_unquoteattrval(hbu_pipe_t pipe) {
  // TODO Check at least one character
  hbu_pipe_accept_while_predicate(pipe, &hbr_unquotedattrval_check);
}

#endif // _HDR_HYPERBUILD_STREAM_HELPER_UNQUOTEATTRVAL
