#ifndef _HDR_HYPERBUILD_STREAM_HELPER_QUOTEATTRVAL
#define _HDR_HYPERBUILD_STREAM_HELPER_QUOTEATTRVAL

#include "../../rule/char/attrvalquote.c"

#include "../../util/hbchar.h"
#include "../../util/buffer.c"
#include "../../util/pipe.c"

#include "../streamoptions.c"

#include "./attrval.h"
#include "./entity.c"

int hbsh_quoteattrval(hbs_options_t so, hbu_pipe_t pipe, int collapse_and_trim_whitespace) {
  int should_remove_attr_quotes = so->remove_attr_quotes;

  hbu_buffer_t value = NULL;
  if (should_remove_attr_quotes) {
    value = hbu_buffer_create();
    hbu_pipe_set_output_redirect(pipe, value);
  }

  hb_char_t quote_char = hbu_pipe_require_skip_predicate(pipe, &hbr_attrvalquote_check, "attribute value quote");
  if (!should_remove_attr_quotes) {
    // Not buffering, so directly output
    hbu_pipe_write(pipe, quote_char);
  }

  int whitespace = 0;
  if (collapse_and_trim_whitespace) {
    hbu_pipe_skip_while_predicate(pipe, &hbr_whitespace_check);
  }

  hb_char_t c;
  int not_empty = 0;
  int can_be_unquoted = 1;

  while ((c = hbu_pipe_peek(pipe)) != quote_char) {
    not_empty = 1;
    can_be_unquoted = can_be_unquoted && hbr_unquotedattrval_check(c);

    if (collapse_and_trim_whitespace && hbr_whitespace_check(c)) {
      whitespace = 1;
      hbu_pipe_skip(pipe);

    } else {
      if (whitespace) {
        whitespace = 0;
        hbu_pipe_write(pipe, ' ');
      }

      if (c == '&') {
        // Call hbsh_entity even if not decoding entities, as it will validate the entity
        hbsh_entity(so, pipe);
      } else {
        hbu_pipe_accept(pipe);
      }
    }
  }

  can_be_unquoted = not_empty && can_be_unquoted;

  hbu_pipe_require_skip(pipe, quote_char);
  if (!should_remove_attr_quotes) {
    // Not buffering, so directly output
    hbu_pipe_write(pipe, quote_char);
    return HBSH_ATTR_QUOTED;

  } else {
    hbu_pipe_set_output_redirect(pipe, NULL);
    if (!can_be_unquoted) hbu_pipe_write(pipe, quote_char);
    hbu_pipe_write_buffer(pipe, value);
    if (!can_be_unquoted) hbu_pipe_write(pipe, quote_char);
    hbu_buffer_destroy(value);
    value = NULL;
    return can_be_unquoted ? HBSH_ATTR_UNQUOTED : HBSH_ATTR_QUOTED;
  }
}

#endif // _HDR_HYPERBUILD_STREAM_HELPER_QUOTEATTRVAL
