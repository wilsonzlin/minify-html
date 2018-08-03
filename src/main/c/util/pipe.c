#ifndef _HDR_HYPERBUILD_UTIL_PIPE
#define _HDR_HYPERBUILD_UTIL_PIPE

#include <string.h>
#include "hbchar.h"
#include "../error/error.c"
#include "buffer.c"
#include "fstreamin.c"
#include "fstreamout.c"

#define MAX_POS_MSG_LEN 1024

typedef int (*hbu_pipe_predicate_t)(hb_char_t);

typedef void (*hbu_pipe_writer_cb_t)(void *, hb_char_t);

typedef struct hbu_pipe_s {
  hbu_fstreamin_t input;
  void *output;
  hbu_pipe_writer_cb_t writer;
  int output_masked;

  hbu_buffer_t buffer;

  int line;
  int column;
  int lastCharWasCR;
  int EOI;
} *hbu_pipe_t;

/*
 *
 * POSITIONING
 *
 */

char *hbu_pipe_generate_pos_msg(hbu_pipe_t pipe) {
  char *msg = malloc(SIZEOF_CHAR * (MAX_POS_MSG_LEN + 1));
  snprintf(msg, MAX_POS_MSG_LEN + 1, "%s [line %d, column %d]", pipe->input->name, pipe->line, pipe->column);
  return msg;
}

/*
 *
 * INTERNAL FUNCTIONS
 *
 */

static hb_eod_char_t _hbu_pipe_read_from_input(hbu_pipe_t pipe) {
  hb_eod_char_t c = hbu_fstreamin_read(pipe->input);
  if (c == HB_EOD) {
    pipe->EOI = 1;
  }
  return c;
}

static void _hbu_pipe_assert_valid_peek_offset(size_t offset) {
  if (offset <= 0) {
    hbe_fatal(HBE_INTERR_PEEK_OFFSET_GEQ_ZERO, "INTERR $offset is less than or equal to zero");
  }
}

static void _hbu_pipe_ensure_buffer(hbu_pipe_t pipe, size_t offset) {
  _hbu_pipe_assert_valid_peek_offset(offset);

  if (pipe->EOI) {
    return;
  }

  size_t current = pipe->buffer->length;

  if (offset <= current) {
    return;
  }

  size_t gap = offset - current;

  for (size_t i = 0; i < gap; i++) {
    hb_eod_char_t c = _hbu_pipe_read_from_input(pipe);
    if (c == HB_EOD) {
      return;
    }
    hbu_buffer_append(pipe->buffer, c);
  }
}

static hb_eod_char_t _hbu_pipe_read_from_buffer_or_input(hbu_pipe_t pipe) {
  if (pipe->EOI) {
    return HB_EOD;
  }

  if (pipe->buffer->length) {
    return hbu_buffer_shift(pipe->buffer);
  }

  return _hbu_pipe_read_from_input(pipe);
}

static void _hbu_pipe_update_pos(hbu_pipe_t pipe, hb_char_t c) {
  switch (c) {
  case '\r':
    pipe->lastCharWasCR = 1;
    pipe->line++;
    pipe->column = 0;
    break;

  case '\n':
    if (!pipe->lastCharWasCR) {
      pipe->line++;
      pipe->column = 0;
    } else {
      pipe->lastCharWasCR = 0;
    }
    break;

  default:
    pipe->column++;
    pipe->lastCharWasCR = 0;
  }
}

static void _hbu_pipe_assert_not_eoi(hbu_pipe_t pipe, hb_eod_char_t c) {
  if (c == HB_EOD) {
    hbe_fatal(HBE_PARSE_UNEXPECTED_END, "Unexpected end of input at %s", hbu_pipe_generate_pos_msg(pipe));
  }
}

static void _hbu_pipe_write_to_output(hbu_pipe_t pipe, hb_char_t c) {
  if (!pipe->output_masked) {
    (*pipe->writer)(pipe->output, c);
  }
}

/*
 *
 * INSTANCE MANAGEMENT FUNCTIONS
 *
 */

hbu_pipe_t hbu_pipe_create(hbu_fstreamin_t input, void *output, hbu_pipe_writer_cb_t writer) {
  hbu_pipe_t pipe = malloc(sizeof(struct hbu_pipe_s));
  pipe->input = input;
  pipe->output = output;
  pipe->writer = writer;
  pipe->output_masked = 0;

  pipe->buffer = hbu_buffer_create();

  pipe->line = 1;
  pipe->column = 0;
  pipe->lastCharWasCR = 0;
  pipe->EOI = 0; // End Of Input

  return pipe;
}

void hbu_pipe_destroy(hbu_pipe_t pipe) {
  hbu_buffer_destroy(pipe->buffer);
  free(pipe);
}

void hbu_pipe_toggle_output_mask(hbu_pipe_t pipe, int output_masked) {
  pipe->output_masked = output_masked;
}

/*
 *
 * BUILDER FUNCTIONS
 *
 */

hbu_pipe_t hbu_pipe_create_blank(void) {
  return hbu_pipe_create(NULL, NULL, NULL);
}

void hbu_pipe_blank_set_input(hbu_pipe_t pipe, hbu_fstreamin_t fstreamin) {
  pipe->input = fstreamin;
}

void hbu_pipe_blank_set_output_fstreamout(hbu_pipe_t pipe, hbu_fstreamout_t fstreamout) {
  pipe->output = fstreamout;
  pipe->writer = (hbu_pipe_writer_cb_t) &hbu_fstreamout_write;
}

void hbu_pipe_blank_set_output_buffer(hbu_pipe_t pipe, hbu_buffer_t buf) {
  pipe->output = buf;
  pipe->writer = (hbu_pipe_writer_cb_t) &hbu_buffer_append;
}

/*
 *
 * CONSUMERS AND PRODUCERS
 *
 */

hb_eod_char_t hbu_pipe_peek_eoi(hbu_pipe_t pipe) {
  // OPTIMISATION: Read directly from buffer if available (no need to unshift later)
  if (pipe->buffer->length) {
    return hbu_buffer_get(pipe->buffer, 0);
  }

  hb_eod_char_t c = _hbu_pipe_read_from_buffer_or_input(pipe);

  if (c != HB_EOD) {
    hbu_buffer_unshift(pipe->buffer, c);
  }

  return c;
}

hb_char_t hbu_pipe_peek(hbu_pipe_t pipe) {
  hb_eod_char_t c = hbu_pipe_peek_eoi(pipe);

  _hbu_pipe_assert_not_eoi(pipe, c);

  return c;
}

hb_char_t hbu_pipe_peek_offset(hbu_pipe_t pipe, size_t offset) {
  _hbu_pipe_ensure_buffer(pipe, offset);

  hb_eod_char_t c = hbu_buffer_get(pipe->buffer, offset - 1);

  _hbu_pipe_assert_not_eoi(pipe, c);

  return c;
}

size_t hbu_pipe_matches(hbu_pipe_t pipe, const char *match) {
  size_t matchlen = strlen(match);

  for (size_t i = 0; i < matchlen; i++) {
    if (hbu_pipe_peek_offset(pipe, i + 1) != match[i]) {
      return 0;
    }
  }

  return matchlen;
}

size_t hbu_pipe_matches_line_terminator(hbu_pipe_t pipe) {
  // `\r\n` must be before `\r`
  return hbu_pipe_matches(pipe, "\r\n") ||
         hbu_pipe_matches(pipe, "\r") ||
         hbu_pipe_matches(pipe, "\n");
}

hb_char_t hbu_pipe_accept(hbu_pipe_t pipe) {
  hb_eod_char_t c = _hbu_pipe_read_from_buffer_or_input(pipe);

  _hbu_pipe_assert_not_eoi(pipe, c);

  _hbu_pipe_update_pos(pipe, c);

  _hbu_pipe_write_to_output(pipe, c);

  return c;
}

void hbu_pipe_accept_count(hbu_pipe_t pipe, size_t count) {
  for (size_t i = 0; i < count; i++) {
    hbu_pipe_accept(pipe);
  }
}

int hbu_pipe_accept_if(hbu_pipe_t pipe, hb_char_t c) {
  hb_eod_char_t n = hbu_pipe_peek_eoi(pipe);

  if (n == HB_EOD || n != c) {
    return 0;
  }

  hbu_pipe_accept(pipe);

  return 1;
}

int hbu_pipe_accept_if_matches(hbu_pipe_t pipe, const char *match) {
  size_t matchedlen = hbu_pipe_matches(pipe, match);

  int matched = matchedlen > 0;

  if (matched) {
    hbu_pipe_accept_count(pipe, matchedlen);
  }

  return matched;
}

int hbu_pipe_accept_if_matches_line_terminator(hbu_pipe_t pipe) {
  size_t matchedlen = hbu_pipe_matches_line_terminator(pipe);

  int matched = matchedlen > 0;

  if (matched) {
    hbu_pipe_accept_count(pipe, matchedlen);
  }

  return matched;
}

int hbu_pipe_accept_if_predicate(hbu_pipe_t pipe, hbu_pipe_predicate_t pred) {
  hb_eod_char_t c = hbu_pipe_peek_eoi(pipe);

  if (c == HB_EOD || !(*pred)(c)) {
    return 0;
  }

  hbu_pipe_accept(pipe);

  return 1;
}

void hbu_pipe_accept_while_predicate(hbu_pipe_t pipe, hbu_pipe_predicate_t pred) {
  while (1) {
    if (!hbu_pipe_accept_if_predicate(pipe, pred)) {
      break;
    }
  }
}

hb_char_t hbu_pipe_skip(hbu_pipe_t pipe) {
  hb_eod_char_t c = _hbu_pipe_read_from_buffer_or_input(pipe);

  _hbu_pipe_assert_not_eoi(pipe, c);

  _hbu_pipe_update_pos(pipe, c);

  return c;
}

void hbu_pipe_skip_amount(hbu_pipe_t pipe, size_t amount) {
  for (size_t i = 0; i < amount; i++) {
    hbu_pipe_skip(pipe);
  }
}

void hbu_pipe_skip_while_predicate(hbu_pipe_t pipe, hbu_pipe_predicate_t pred) {
  while (1) {
    hb_eod_char_t c = hbu_pipe_peek_eoi(pipe);

    if (c == HB_EOD || !(*pred)(c)) {
      break;
    }

    hbu_pipe_skip(pipe);
  }
}

int hbu_pipe_skip_if_matches(hbu_pipe_t pipe, const char *match) {
  size_t matchedlen = hbu_pipe_matches(pipe, match);

  int matched = matchedlen > 0;

  if (matched) {
    hbu_pipe_skip_amount(pipe, matchedlen);
  }

  return matched;
}

void hbu_pipe_require(hbu_pipe_t pipe, hb_char_t c) {
  hb_char_t n = hbu_pipe_accept(pipe);

  if (c != n) {
    hbe_fatal(HBE_PARSE_EXPECTED_NOT_FOUND, "Expected `%c` (0x%x), got `%c` (0x%x) at %s", c, c, n, n, hbu_pipe_generate_pos_msg(pipe));
  }
}

hb_char_t hbu_pipe_require_skip(hbu_pipe_t pipe, hb_char_t c) {
  hb_char_t n = hbu_pipe_skip(pipe);

  if (c != n) {
    hbe_fatal(HBE_PARSE_EXPECTED_NOT_FOUND, "Expected `%c` (0x%x), got `%c` (0x%x) at %s", c, c, n, n, hbu_pipe_generate_pos_msg(pipe));
  }

  return n;
}

hb_char_t hbu_pipe_require_predicate(hbu_pipe_t pipe, hbu_pipe_predicate_t pred, const char *name) {
  hb_char_t n = hbu_pipe_accept(pipe);

  if (!(*pred)(n)) {
    hbe_fatal(HBE_PARSE_EXPECTED_NOT_FOUND, "Expected %s, got `%c` (0x%x) at %s", name, n, n, hbu_pipe_generate_pos_msg(pipe));
  }

  return n;
}

void hbu_pipe_require_match(hbu_pipe_t pipe, const char *match) {
  if (!hbu_pipe_matches(pipe, match)) {
    hbe_fatal(HBE_PARSE_EXPECTED_NOT_FOUND, "Expected %s at %s", match, hbu_pipe_generate_pos_msg(pipe));
  }
}

void hbu_pipe_warn(hbu_pipe_t pipe, const char *msg) {
  hbe_warn("%s at %s", msg, hbu_pipe_generate_pos_msg(pipe));
}

void hbu_pipe_debug(hbu_pipe_t pipe, const char *msg) {
  hbe_debug("%s at %s", msg, hbu_pipe_generate_pos_msg(pipe));
}

// Don't use this function for error calls in this file, as this function doesn't support varargs
void hbu_pipe_error(hbu_pipe_t pipe, hbe_errcode_t errcode, const char *reason) {
  hbe_fatal(errcode, "%s at %s", reason, hbu_pipe_generate_pos_msg(pipe));
}

void hbu_pipe_write(hbu_pipe_t pipe, hb_char_t c) {
  _hbu_pipe_write_to_output(pipe, c);
}

#endif
