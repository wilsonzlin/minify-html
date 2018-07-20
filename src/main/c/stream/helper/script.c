#ifndef _HDR_HYPERBUILD_STREAM_HELPER_SCRIPT
#define _HDR_HYPERBUILD_STREAM_HELPER_SCRIPT

#include "../../error/error.c"
#include "../../util/hbchar.h"
#include "../../util/pipe.c"

static void _hbsh_script_slcomment(hbu_pipe_t pipe) {
  hbu_pipe_require_match(pipe, "//");

  while (!hbu_pipe_accept_if_matches_line_terminator(pipe)) {
    hbu_pipe_accept(pipe);
  }
}

static void _hbsh_script_mlcomment(hbu_pipe_t pipe) {
  hbu_pipe_require_match(pipe, "/*");

  while (!hbu_pipe_accept_if_matches(pipe, "*/")) {
    hbu_pipe_accept(pipe);
  }
}

static void _hbsh_script_string(hbu_pipe_t pipe) {
  hb_char_t delim = hbu_pipe_accept(pipe);

  if (delim != '"' && delim != '\'') {
    hbu_pipe_error(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected JavaScript string delimiter");
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
        hbu_pipe_error(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Unterminated JavaScript string");
      }
    }

    escaping = 0;
  }
}

static void _hbsh_script_template(hbu_pipe_t pipe) {
  hbu_pipe_require_match(pipe, "`");

  int escaping = 0;

  while (1) {
    hb_char_t c = hbu_pipe_accept(pipe);

    if (c == '\\') {
      escaping ^= 1;
      continue;
    }

    if (c == '`' && !escaping) {
      break;
    }

    escaping = 0;
  }
}

void hbsh_script(hbu_pipe_t pipe) {
  while (!hbu_pipe_matches(pipe, "</")) {
    if (hbu_pipe_matches(pipe, "//")) {
      _hbsh_script_slcomment(pipe);

    } else if (hbu_pipe_matches(pipe, "/*")) {
      _hbsh_script_mlcomment(pipe);

    } else if (hbu_pipe_matches(pipe, "\"")) {
      _hbsh_script_string(pipe);

    } else if (hbu_pipe_matches(pipe, "'")) {
      _hbsh_script_string(pipe);

    } else if (hbu_pipe_matches(pipe, "`")) {
      _hbsh_script_template(pipe);

    } else {
      hbu_pipe_accept(pipe);
    }
  }
}

#endif // _HDR_HYPERBUILD_STREAM_HELPER_SCRIPT
