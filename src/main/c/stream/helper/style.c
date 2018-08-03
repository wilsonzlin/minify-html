#ifndef _HDR_HYPERBUILD_STREAM_HELPER_STYLE
#define _HDR_HYPERBUILD_STREAM_HELPER_STYLE

#include "../../error/error.c"
#include "../../util/hbchar.h"
#include "../../util/pipe.c"

static void _hbsh_style_comment(hbu_pipe_t pipe) {
  hbu_pipe_require_match(pipe, "/*");

  while (!hbu_pipe_accept_if_matches(pipe, "*/")) {
    hbu_pipe_accept(pipe);
  }
}

static void _hbsh_style_string(hbu_pipe_t pipe) {
  hb_char_t delim = hbu_pipe_accept(pipe);

  if (delim != '"' && delim != '\'') {
    hbu_pipe_error(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected CSS string delimiter");
  }

  int escaping = 0;

  while (1) {
    hb_char_t c = hbu_pipe_accept(pipe);

    if (c == '\\') {
      escaping ^= 1;
      continue;
    }

    if (c == delim && !escaping) {
      break;
    }

    if (hbu_pipe_accept_if_matches_line_terminator(pipe)) {
      if (!escaping) {
        hbu_pipe_error(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Unterminated CSS string");
      }
    }

    escaping = 0;
  }
}

void hbsh_style(hbu_pipe_t pipe) {
  while (!hbu_pipe_matches(pipe, "</")) {
    if (hbu_pipe_matches(pipe, "/*")) {
      _hbsh_style_comment(pipe);

    } else if (hbu_pipe_peek(pipe) == '"' || hbu_pipe_peek(pipe) == '\'') {
      _hbsh_style_string(pipe);

    } else {
      hbu_pipe_accept(pipe);
    }
  }
}

#endif // _HDR_HYPERBUILD_STREAM_HELPER_STYLE
