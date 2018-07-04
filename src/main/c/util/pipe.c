#ifndef _HDR_HYPERBUILD_UTIL_PIPE
#define _HDR_HYPERBUILD_UTIL_PIPE

#include "hbchar.h"
#include "../error/error.c"
#include "buffer.c"
#include "fstreamin.c"
#include "fstreamout.c"

#define MAX_POS_MSG_LEN 1024

typedef void (*hbu_pipe_writer_cb_t)(void *, hb_char_t);

typedef struct hbu_pipe_s {
  hbu_fstreamin_t input;
  void *output;
  hbu_pipe_writer_cb_t writer;

  hbu_buffer_t buffer;

  int line;
  int column;
  int lastCharWasCR;
  int EOI;
} *hbu_pipe_t;

static hb_eod_char_t _hbu_pipe_read_from_buffer_or_input(hbu_pipe_t pipe) {
  if (pipe->EOI) {
    return HB_EOD;
  }

  if (pipe->buffer->length) {
    return hbu_buffer_shift(pipe->buffer);
  }

  hb_eod_char_t c = hbu_fstreamin_read(pipe->input);
  if (c == HB_EOD) {
    pipe->EOI = 1;
  }
  return c;
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

static char *_hbu_pipe_generate_pos_msg(hbu_pipe_t pipe) {
  char *msg = malloc(SIZEOF_CHAR * (MAX_POS_MSG_LEN + 1));
  snprintf(msg, MAX_POS_MSG_LEN + 1, "%s [line %d, column %d]", pipe->input->name, pipe->line, pipe->column);
  return msg;
}

static void _hbu_pipe_assert_not_eoi(hbu_pipe_t pipe, hb_eod_char_t c) {
  if (c == HB_EOD) {
    hbe_fatal("Unexpected end of input at %s", _hbu_pipe_generate_pos_msg(pipe));
  }
}

static void _hbu_pipe_write_to_output(hbu_pipe_t pipe, hb_char_t c) {
  (*pipe->writer)(pipe->output, c);
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

hb_char_t hbu_pipe_accept(hbu_pipe_t pipe) {
  hb_eod_char_t c = _hbu_pipe_read_from_buffer_or_input(pipe);

  _hbu_pipe_assert_not_eoi(pipe, c);

  _hbu_pipe_update_pos(pipe, c);

  _hbu_pipe_write_to_output(pipe, c);

  return c;
}

void hbu_pipe_skip(hbu_pipe_t pipe) {
  hb_eod_char_t c = _hbu_pipe_read_from_buffer_or_input(pipe);

  _hbu_pipe_assert_not_eoi(pipe, c);

  _hbu_pipe_update_pos(pipe, c);
}

hb_char_t hbu_pipe_peek(hbu_pipe_t pipe) {
  hb_eod_char_t c = _hbu_pipe_read_from_buffer_or_input(pipe);

  _hbu_pipe_assert_not_eoi(pipe, c);

  hbu_buffer_unshift(pipe->buffer, c);

  return c;
}

void hbu_pipe_write(hbu_pipe_t pipe, hb_char_t c) {
  _hbu_pipe_write_to_output(pipe, c);
}

#endif
