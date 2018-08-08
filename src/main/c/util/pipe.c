#ifndef _HDR_HYPERBUILD_UTIL_PIPE
#define _HDR_HYPERBUILD_UTIL_PIPE

#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include "hbchar.h"
#include "mem.c"
#include "../error/error.c"
#include "buffer.c"
#include "fstreamin.c"
#include "fstreamout.c"

#define MAX_POS_MSG_LEN 1024
#define MAX_ERR_MSG_LEN 1024

typedef int (*hbu_pipe_predicate_t)(hb_char_t);

typedef void (*hbu_pipe_writer_cb_t)(void *, hb_char_t);

typedef struct hbu_pipe_s {
  hbu_fstreamin_t input;
  void *output;
  hbu_pipe_writer_cb_t writer;

  int output_masked;
  hbu_buffer_t output_redirect;

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
  char *msg = hbu_mem_calloc(MAX_POS_MSG_LEN + 1, SIZEOF_CHAR);
  snprintf(msg, MAX_POS_MSG_LEN, "%s [line %d, column %d]", pipe->input->name, pipe->line, pipe->column);
  return msg;
}

/*
 *
 * MESSAGING
 *
 */

/**
 * Generates a warning message with the current position appended.
 *
 * @param pipe pipe
 * @param msg message
 */
void hbu_pipe_warn(hbu_pipe_t pipe, const char *msg) {
  hbe_warn("%s at %s", msg, hbu_pipe_generate_pos_msg(pipe));
}

/**
 * Generates a debug message with the current position appended.
 *
 * @param pipe pipe
 * @param msg message
 */
void hbu_pipe_debug(hbu_pipe_t pipe, const char *msg) {
  hbe_debug("%s at %s", msg, hbu_pipe_generate_pos_msg(pipe));
}

/**
 * Exits with an error using a message with the current position appended.
 *
 * @param pipe pipe
 * @param errcode error code
 * @param reason message
 */
void hbu_pipe_error(hbu_pipe_t pipe, hbe_errcode_t errcode, const char *reason, ...) {
  va_list args;
  va_start(args, reason);

  char *msg = hbu_mem_calloc(MAX_ERR_MSG_LEN + 1, SIZEOF_CHAR);
  vsnprintf(msg, MAX_ERR_MSG_LEN, reason, args);
  hbe_fatal(errcode, "%s at %s", msg, hbu_pipe_generate_pos_msg(pipe));

  va_end(args);
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
      // EOI flag already set by _hbu_pipe_read_from_input
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
    hbu_pipe_error(pipe, HBE_PARSE_UNEXPECTED_END, "Unexpected end of input");
  }
}

static void _hbu_pipe_write_to_output(hbu_pipe_t pipe, hb_char_t c) {
  if (!pipe->output_masked) {
    hbu_buffer_t redirect = pipe->output_redirect;
    if (redirect != NULL) {
      hbu_buffer_append(redirect, c);
    } else {
      (*pipe->writer)(pipe->output, c);
    }
  }
}

/*
 *
 * INSTANCE MANAGEMENT FUNCTIONS
 *
 */

hbu_pipe_t hbu_pipe_create(hbu_fstreamin_t input, void *output, hbu_pipe_writer_cb_t writer) {
  hbu_pipe_t pipe = hbu_mem_malloc(sizeof(struct hbu_pipe_s));
  pipe->input = input;
  pipe->output = output;
  pipe->writer = writer;
  pipe->output_masked = 0;
  pipe->output_redirect = NULL;

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

/**
 * Enables or disables the output mask.
 * When the output mask is enabled, all writes are simply discarded and not actually written to output.
 *
 * @param pipe pipe
 * @param output_masked 1 to enable, 0 to disable
 * @return previous state
 */
int hbu_pipe_toggle_output_mask(hbu_pipe_t pipe, int output_masked) {
  int current = pipe->output_masked;
  pipe->output_masked = output_masked;
  return current;
}

/**
 * Enables or disables the output redirect.
 * When the output redirect is enabled, all writes are written to a buffer instead of the output.
 *
 * @param pipe pipe
 * @param output_redirect buffer to redirect writes to, or NULL to disable
 */
void hbu_pipe_set_output_redirect(hbu_pipe_t pipe, hbu_buffer_t output_redirect) {
  pipe->output_redirect = output_redirect;
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

/**
 * Gets the next character.
 * If it's the end, {@link HB_EOD} is returned.
 *
 * @param pipe pipe
 * @return character or {@link HB_EOD}
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

/**
 * Gets the next character.
 * Will cause an error if it's the end and there is no next character.
 *
 * @param pipe pipe
 * @return character
 */
hb_char_t hbu_pipe_peek(hbu_pipe_t pipe) {
  hb_eod_char_t c = hbu_pipe_peek_eoi(pipe);

  _hbu_pipe_assert_not_eoi(pipe, c);

  return c;
}

/**
 * Gets the <i>n</i>th character from current, where <i>n</i> is <code>offset</code>.
 * When <code>offset</code> is 1, the next character is returned (equivalent to {@link hbu_pipe_peek_eoi_offset}).
 * If <code>offset</code> is after the last character, {@link HB_EOD} is returned.
 *
 * @param pipe pipe
 * @param offset position of character to get
 * @return character or {@link HB_EOD}
 */
hb_eod_char_t hbu_pipe_peek_eoi_offset(hbu_pipe_t pipe, size_t offset) {
  _hbu_pipe_ensure_buffer(pipe, offset);

  hb_eod_char_t c = hbu_buffer_get(pipe->buffer, offset - 1);

  return c;
}

/**
 * Gets the <i>n</i>th character from current, where <i>n</i> is <code>offset</code>.
 * When <code>offset</code> is 1, the next character is returned (equivalent to {@link hbu_pipe_peek_offset}).
 * An error will be caused if <code>offset</code> is after the last character.
 *
 * @param pipe pipe
 * @param offset position of character to get
 * @return character
 */
hb_char_t hbu_pipe_peek_offset(hbu_pipe_t pipe, size_t offset) {
  hb_eod_char_t c = hbu_pipe_peek_eoi_offset(pipe, offset);

  _hbu_pipe_assert_not_eoi(pipe, c);

  return c;
}

/**
 * Checks if the next sequence of characters is the null-terminated character array <code>match</code>.
 * Won't cause an error if insufficient amount of characters left.
 *
 * @param pipe pipe
 * @param match characters to match
 * @return amount of characters matched, which should be equal to <code>strlen(match)</code>
 */
size_t hbu_pipe_matches(hbu_pipe_t pipe, const char *match) {
  size_t matchlen = strlen(match);

  for (size_t i = 0; i < matchlen; i++) {
    hb_eod_char_t c = hbu_pipe_peek_eoi_offset(pipe, i + 1);
    if (c == HB_EOD || c != match[i]) {
      return 0;
    }
  }

  return matchlen;
}

/**
 * Checks if the next sequence of characters matches the null-terminated character array <code>match</code>
 * of lowercase characters case-insensitively.
 * Won't cause an error if insufficient amount of characters left.
 *
 * @param pipe pipe
 * @param match characters to match case-insensitively
 * @return amount of characters matched, which should be equal to <code>strlen(match)</code>
 */
size_t hbu_pipe_matches_i(hbu_pipe_t pipe, const char *match) {
  size_t matchlen = strlen(match);

  for (size_t i = 0; i < matchlen; i++) {
    hb_eod_char_t c = hbu_pipe_peek_eoi_offset(pipe, i + 1);
    if (!(
      c != HB_EOD &&
      (
        c == match[i] ||
        (hbr_ucalpha_check(c) && (c + 32) == match[i])
      )
    )) {
      return 0;
    }
  }

  return matchlen;
}

/**
 * Checks if the next sequence of characters is "\r", "\n", or "\r\n".
 * Won't cause an error if insufficient amount of characters left.
 *
 * @param pipe pipe
 * @return amount of characters matched
 */
size_t hbu_pipe_matches_line_terminator(hbu_pipe_t pipe) {
  // `\r\n` must be before `\r`
  return hbu_pipe_matches(pipe, "\r\n") ||
         hbu_pipe_matches(pipe, "\r") ||
         hbu_pipe_matches(pipe, "\n");
}

/**
 * Accepts the next character.
 * Will cause an error if already at end.
 *
 * @param pipe pipe
 * @return next character
 */
hb_char_t hbu_pipe_accept(hbu_pipe_t pipe) {
  hb_eod_char_t c = _hbu_pipe_read_from_buffer_or_input(pipe);

  _hbu_pipe_assert_not_eoi(pipe, c);

  _hbu_pipe_update_pos(pipe, c);

  _hbu_pipe_write_to_output(pipe, c);

  return c;
}

/**
 * Accepts the next <code>count</code> characters.
 * Requires at least <code>count</code> characters remaining.
 *
 * @param pipe pipe
 * @param count amount of characters
 */
void hbu_pipe_accept_count(hbu_pipe_t pipe, size_t count) {
  for (size_t i = 0; i < count; i++) {
    hbu_pipe_accept(pipe);
  }
}

/**
 * Accepts the following character if it is <code>c</code>.
 * Won't match or cause an error if there are no characters remaining.
 *
 * @param pipe pipe
 * @param c character to match
 * @return 0 if nothing was accepted, 1 otherwise
 */
int hbu_pipe_accept_if(hbu_pipe_t pipe, hb_char_t c) {
  hb_eod_char_t n = hbu_pipe_peek_eoi(pipe);

  if (n == HB_EOD || n != c) {
    return 0;
  }

  hbu_pipe_accept(pipe);

  return 1;
}

/**
 * Accepts the following characters if they match <code>match</code>.
 * Won't match or cause an error if there are not enough characters remaining.
 *
 * @param pipe pipe
 * @param match characters to match
 * @return 0 if nothing was accepted, 1 otherwise
 */
int hbu_pipe_accept_if_matches(hbu_pipe_t pipe, const char *match) {
  size_t matchedlen = hbu_pipe_matches(pipe, match);

  int matched = matchedlen > 0;

  if (matched) {
    hbu_pipe_accept_count(pipe, matchedlen);
  }

  return matched;
}

/**
 * Accepts the following characters if they are either "\r", "\r\n", or "\n".
 * Won't cause an error if insufficient amount of characters left.
 *
 * @param pipe pipe
 * @return amount of characters matched
 */
size_t hbu_pipe_accept_if_matches_line_terminator(hbu_pipe_t pipe) {
  size_t matchedlen = hbu_pipe_matches_line_terminator(pipe);

  if (matchedlen) {
    hbu_pipe_accept_count(pipe, matchedlen);
  }

  return matchedlen;
}

/**
 * Accepts the following character if it satisfies the predicate <code>pred</code>.
 * Won't do anything if already at the end.
 *
 * @param pipe pipe
 * @param pred predicate
 * @return 0 if nothing was accepted, 1 otherwise
 */
int hbu_pipe_accept_if_predicate(hbu_pipe_t pipe, hbu_pipe_predicate_t pred) {
  hb_eod_char_t c = hbu_pipe_peek_eoi(pipe);

  if (c == HB_EOD || !(*pred)(c)) {
    return 0;
  }

  hbu_pipe_accept(pipe);

  return 1;
}

/**
 * Accepts every following character until one dissatisfies the predicate <code>pred</code>,
 * or the end is reached.
 *
 * @param pipe pipe
 * @param pred predicate
 * @return amount of characters accepted
 */
size_t hbu_pipe_accept_while_predicate(hbu_pipe_t pipe, hbu_pipe_predicate_t pred) {
  size_t count = 0;

  while (1) {
    if (!hbu_pipe_accept_if_predicate(pipe, pred)) {
      break;
    }
    count++;
  }

  return count;
}

/**
 * Skips over the next character.
 * Requires that the file has at least one character remaining.
 *
 * @param pipe pipe
 * @return skipped character
 */
hb_char_t hbu_pipe_skip(hbu_pipe_t pipe) {
  hb_eod_char_t c = _hbu_pipe_read_from_buffer_or_input(pipe);

  _hbu_pipe_assert_not_eoi(pipe, c);

  _hbu_pipe_update_pos(pipe, c);

  return c;
}

/**
 * Skips over the next <code>amount</code> characters.
 * Requires that the file has at least <code>amount</code> characters remaining.
 *
 * @param pipe pipe
 * @param amount amount of characters to skip
 */
void hbu_pipe_skip_amount(hbu_pipe_t pipe, size_t amount) {
  for (size_t i = 0; i < amount; i++) {
    hbu_pipe_skip(pipe);
  }
}

/**
 * Skips over every following character until one dissatisfies the predicate <code>pred</code>,
 * or the end is reached.
 *
 * @param pipe pipe
 * @param pred predicate
 * @return amount of characters skipped
 */
size_t hbu_pipe_skip_while_predicate(hbu_pipe_t pipe, hbu_pipe_predicate_t pred) {
  size_t count = 0;

  while (1) {
    hb_eod_char_t c = hbu_pipe_peek_eoi(pipe);

    if (c == HB_EOD || !(*pred)(c)) {
      break;
    }

    hbu_pipe_skip(pipe);
    count++;
  }

  return count;
}

/**
 * Skips over the next sequence of characters if they are <code>match</code>.
 * Requires that the file has at least the length of <code>match</code> characters remaining.
 *
 * @param pipe pipe
 * @param match sequence of characters to match
 * @return 0 if not matched, 1 otherwise
 */
int hbu_pipe_skip_if_matches(hbu_pipe_t pipe, const char *match) {
  size_t matchedlen = hbu_pipe_matches(pipe, match);

  int matched = matchedlen > 0;

  if (matched) {
    hbu_pipe_skip_amount(pipe, matchedlen);
  }

  return matched;
}

/**
 * Requires the next character to be <code>c</code>.
 * The matched character is written to output.
 *
 * @param pipe pipe
 * @param c character to match
 */
void hbu_pipe_require(hbu_pipe_t pipe, hb_char_t c) {
  hb_char_t n = hbu_pipe_accept(pipe);

  if (c != n) {
    hbu_pipe_error(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected `%c` (0x%x), got `%c` (0x%x)", c, c, n, n);
  }
}

/**
 * Requires the next character to be <code>c</code>.
 * The matched character is skipped over and NOT written to output, and also returned.
 *
 * @param pipe pipe
 * @param c character to match
 * @return matched character
 */
hb_char_t hbu_pipe_require_skip(hbu_pipe_t pipe, hb_char_t c) {
  hb_char_t n = hbu_pipe_skip(pipe);

  if (c != n) {
    hbu_pipe_error(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected `%c` (0x%x), got `%c` (0x%x) at %s", c, c, n, n);
  }

  return n;
}

/**
 * Requires the next character to satisfy the predicate <code>pred</code>.
 * The matched character is written to output.
 * If not matched, the error message will describe the expected output using <code>name</code>.
 *
 * @param pipe pipe
 * @param pred predicate
 * @param name what to output in the error message to describe the requirement
 * @return required character
 */
hb_char_t hbu_pipe_require_predicate(hbu_pipe_t pipe, hbu_pipe_predicate_t pred, const char *name) {
  hb_char_t n = hbu_pipe_accept(pipe);

  if (!(*pred)(n)) {
    hbu_pipe_error(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected %s, got `%c` (0x%x)", name, n, n);
  }

  return n;
}

/**
 * Requires the next sequence of characters to be equal to <code>match</code>.
 * Matched characters are written to output.
 *
 * @param pipe pipe
 * @param match sequence of characters to require
 */
void hbu_pipe_require_match(hbu_pipe_t pipe, const char *match) {
  if (!hbu_pipe_accept_if_matches(pipe, match)) {
    hbu_pipe_error(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected `%s`", match);
  }
}

/**
 * Requires the next sequence of characters to be equal to <code>match</code>.
 * Matched characters are skipped over and NOT written to output.
 *
 * @param pipe pipe
 * @param match sequence of characters to require
 */
void hbu_pipe_require_skip_match(hbu_pipe_t pipe, const char *match) {
  if (!hbu_pipe_skip_if_matches(pipe, match)) {
    hbu_pipe_error(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected `%s`", match);
  }
}

/**
 * Writes a character.
 *
 * @param pipe pipe
 * @param c character
 */
void hbu_pipe_write(hbu_pipe_t pipe, hb_char_t c) {
  _hbu_pipe_write_to_output(pipe, c);
}

/**
 * Writes a character array until a NUL character is reached.
 *
 * @param pipe pipe
 * @param str null-terminated character array
 */
void hbu_pipe_write_str(hbu_pipe_t pipe, hb_char_t *str) {
  hb_char_t c;
  for (size_t i = 0; (c = str[i]); i++) {
    _hbu_pipe_write_to_output(pipe, c);
  }
}

/**
 * Writes <code>len</code> characters from the beginning of a character array.
 *
 * @param pipe pipe
 * @param str character array
 * @param len amount of characters to write
 */
void hbu_pipe_write_str_len(hbu_pipe_t pipe, hb_char_t *str, size_t len) {
  for (size_t i = 0; i < len; i++) {
    _hbu_pipe_write_to_output(pipe, str[i]);
  }
}

/**
 * Writes a buffer.
 *
 * @param pipe pipe
 * @param buffer buffer
 */
void hbu_pipe_write_buffer(hbu_pipe_t pipe, hbu_buffer_t buffer) {
  for (size_t i = 0; i < buffer->length; i++) {
    _hbu_pipe_write_to_output(pipe, hbu_buffer_get(buffer, i));
  }
}

/**
 * Writes a character from its Unicode code point in UTF-8 encoding, which may be multiple bytes.
 *
 * @param pipe pipe
 * @param code_point Unicode code point
 * @return amount of bytes written (0 if invalid code point)
 */
// Logic copied from https://gist.github.com/MightyPork/52eda3e5677b4b03524e40c9f0ab1da5
int hbu_pipe_write_unicode(hbu_pipe_t pipe, uint32_t code_point) {
   if (code_point <= 0x7F) {
    // Plain ASCII
    hbu_pipe_write(pipe, (hb_char_t) code_point);
    return 1;
  }

  if (code_point <= 0x07FF) {
    // 2-byte unicode
    hbu_pipe_write(pipe, (hb_char_t) (((code_point >> 6) & 0x1F) | 0xC0));
    hbu_pipe_write(pipe, (hb_char_t) (((code_point >> 0) & 0x3F) | 0x80));
    return 2;
  }

  if (code_point <= 0xFFFF) {
    // 3-byte unicode
    hbu_pipe_write(pipe, (hb_char_t) (((code_point >> 12) & 0x0F) | 0xE0));
    hbu_pipe_write(pipe, (hb_char_t) (((code_point >>  6) & 0x3F) | 0x80));
    hbu_pipe_write(pipe, (hb_char_t) (((code_point >>  0) & 0x3F) | 0x80));
    return 3;
  }

  if (code_point <= 0x10FFFF) {
    // 4-byte unicode
    hbu_pipe_write(pipe, (hb_char_t) (((code_point >> 18) & 0x07) | 0xF0));
    hbu_pipe_write(pipe, (hb_char_t) (((code_point >> 12) & 0x3F) | 0x80));
    hbu_pipe_write(pipe, (hb_char_t) (((code_point >>  6) & 0x3F) | 0x80));
    hbu_pipe_write(pipe, (hb_char_t) (((code_point >>  0) & 0x3F) | 0x80));
    return 4;
  }

  return 0;
}

#endif
