#include <string.h>
#include "../../rule/char/ucalpha.c"
#include "../char/char.c"
#include "../execution/error.c"
#include "../list/char.c"
#include "../fstream/fstreamin.c"
#include "../fstream/fstreamout.c"

#define HBU_PIPE_MAX_ERR_MSG_LEN 1024

// Use macro to prevent having to allocate (and therefore free/manage) memory
#define HBU_FN_FORMAT_WITH_POS(fn, a, format, ...) fn(a, format " at %s [line %d, column %d]", __VA_ARGS__, pipe->input_name, pipe->line, pipe->column);

#define HBU_PIPE_THROW(pipe, errcode, format, ...) *hbe_err = hbu_pipe_error(pipe, errcode, format, ##__VA_ARGS__); return 0;
#define HBU_PIPE_THROW_V(pipe, errcode, format, ...) *hbe_err = hbu_pipe_error(pipe, errcode, format, ##__VA_ARGS__); return;
#define HBU_PIPE_THROW_F(pipe, errcode, format, ...) *hbe_err = hbu_pipe_error(pipe, errcode, format, ##__VA_ARGS__); goto finally;

typedef int (*hbu_pipe_predicate_t)(hb_char_t);

typedef hb_eod_char_t (*hbu_pipe_reader_cb_t)(hbe_err_t *hbe_err, void *);
typedef void (*hbu_pipe_writer_cb_t)(hbe_err_t *hbe_err, void *, hb_char_t);

typedef struct hbu_pipe_s {
  void* input;
  hbu_pipe_reader_cb_t reader;
  void *output;
  hbu_pipe_writer_cb_t writer;

  char *input_name;

  int output_masked;
  hbu_list_char_t output_redirect;

  hbu_list_char_t buffer;

  int line;
  int column;
  int lastCharWasCR;
  int EOI;
} *hbu_pipe_t;

/*
 *
 * MESSAGING
 *
 */

/**
 * Logs a debug message with the current position appended.
 *
 * @param pipe pipe
 * @param msg message
 */
void hbu_pipe_debug(hbu_pipe_t pipe, const char *msg) {
  HBU_FN_FORMAT_WITH_POS(hbl_log, HBL_LOG_DEBUG, "%s", msg);
}

/**
 * Logs a warning message with the current position appended.
 *
 * @param pipe pipe
 * @param msg message
 */
void hbu_pipe_warn(hbu_pipe_t pipe, const char *msg) {
  HBU_FN_FORMAT_WITH_POS(hbl_log, HBL_LOG_WARN, "%s", msg);
}

/**
 * Creates an error using a message with the current position appended.
 *
 * @param pipe pipe
 * @param errcode error code
 * @param reason message
 * @return error
 */
hbe_err_t hbu_pipe_error(hbu_pipe_t pipe, hbe_errcode_t errcode, const char *reason, ...) {
  va_list args;
  va_start(args, reason);

  char *msg = calloc(HBU_PIPE_MAX_ERR_MSG_LEN + 1, SIZEOF_CHAR);
  vsnprintf(msg, HBU_PIPE_MAX_ERR_MSG_LEN, reason, args);

  va_end(args);

  hbe_err_t err = HBU_FN_FORMAT_WITH_POS(hbe_err_create, errcode, "%s", msg);
  free(msg);

  return err;
}

/*
 *
 * INTERNAL FUNCTIONS
 *
 */

/**
 * Reads from input.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @throws on read error
 */
static hb_eod_char_t _hbu_pipe_read_from_input(hbe_err_t *hbe_err, hbu_pipe_t pipe) {
  hb_eod_char_t c = HBE_CATCH((*pipe->reader), pipe->input);
  if (c == HB_EOD) {
    pipe->EOI = 1;
  }
  return c;
}

/**
 * Ensures the buffer has loaded at least the next <code>offset</code> characters, or the remaining
 * characters from input if there are less than <code>offset</code> characters left.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @throws on read error
 */
static void _hbu_pipe_ensure_buffer(hbe_err_t *hbe_err, hbu_pipe_t pipe, size_t offset) {
  if (pipe->EOI) {
    return;
  }

  size_t current = pipe->buffer->length;

  if (offset <= current) {
    return;
  }

  size_t gap = offset - current;

  for (size_t i = 0; i < gap; i++) {
    hb_eod_char_t c = HBE_CATCH_V(_hbu_pipe_read_from_input, pipe);
    if (c == HB_EOD) {
      // EOI flag already set by _hbu_pipe_read_from_input
      return;
    }
    hbu_list_char_append(pipe->buffer, c);
  }
}

/**
 * Gets the next character, whether it's by shifting the buffer or reading from input.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @throws on read error
 */
static hb_eod_char_t _hbu_pipe_read_from_buffer_or_input(hbe_err_t *hbe_err, hbu_pipe_t pipe) {
  if (pipe->EOI) {
    return HB_EOD;
  }

  if (pipe->buffer->length) {
    return hbu_list_char_shift(pipe->buffer);
  }

  return HBE_CATCH(_hbu_pipe_read_from_input, pipe);
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

/**
 * Ensures that the provided character is not @{link HB_EOD}, and causes an error otherwise.
 *
 * @param c character to test for <code>HB_EOD</code>
 * @throws HBE_PARSE_UNEXPECTED_END if <code>c</code> is <code>HB_EOD</code>
 */
static void _hbu_pipe_assert_not_eoi(hbe_err_t *hbe_err, hb_eod_char_t c) {
  if (c == HB_EOD) {
    HBE_THROW_V(HBE_PARSE_UNEXPECTED_END, "Unexpected end of input");
  }
}

/**
 * Writes a character to the redirect, if enabled, otherwise output, of a pipe,
 * unless the output is masked.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param c character to write
 * @return a freshly-created pipe
 * @throws on write error
 */
static void _hbu_pipe_write_to_output(hbe_err_t *hbe_err, hbu_pipe_t pipe, hb_char_t c) {
  if (!pipe->output_masked) {
    hbu_list_char_t redirect = pipe->output_redirect;
    if (redirect != NULL) {
      hbu_list_char_append(redirect, c);
    } else {
      HBE_CATCH_V((*pipe->writer), pipe->output, c);
    }
  }
}

/*
 *
 * INSTANCE MANAGEMENT FUNCTIONS
 *
 */

/**
 * Allocates memory for a pipe, and creates one with provided arguments.
 *
 * @param input input
 * @param reader reader
 * @param input_name input_name
 * @param output output
 * @param writer writer
 * @return a freshly-created pipe
 */
hbu_pipe_t hbu_pipe_create(void *input, hbu_pipe_reader_cb_t reader, char *input_name, void *output, hbu_pipe_writer_cb_t writer) {
  hbu_pipe_t pipe = malloc(sizeof(struct hbu_pipe_s));
  pipe->input = input;
  pipe->reader = reader;
  pipe->output = output;
  pipe->writer = writer;

  pipe->input_name = input_name;

  pipe->output_masked = 0;
  pipe->output_redirect = NULL;

  pipe->buffer = hbu_list_char_create();

  pipe->line = 1;
  pipe->column = 0;
  pipe->lastCharWasCR = 0;
  pipe->EOI = 0; // End Of Input

  return pipe;
}

/**
 * Frees all memory associated with a pipe.
 *
 * @param pipe pipe
 */
void hbu_pipe_destroy(hbu_pipe_t pipe) {
  hbu_list_char_destroy(pipe->buffer);
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
void hbu_pipe_set_output_redirect(hbu_pipe_t pipe, hbu_list_char_t output_redirect) {
  pipe->output_redirect = output_redirect;
}

/*
 *
 * BUILDER FUNCTIONS
 *
 */

hbu_pipe_t hbu_pipe_create_blank(char *input_name) {
  return hbu_pipe_create(NULL, NULL, input_name, NULL, NULL);
}

void hbu_pipe_blank_set_input_fstreamin(hbu_pipe_t pipe, hbu_fstreamin_t fstreamin) {
  pipe->input = fstreamin;
  pipe->reader = (hbu_pipe_reader_cb_t) &hbu_fstreamin_read;
}

// Wrapper function for hbu_list_char_shift to make it compatible with hbu_pipe_reader_cb_t
hb_eod_char_t hbu_pipe_read_from_list_char_input(hbe_err_t *hbe_err, hbu_list_char_t input) {
  (void) hbe_err;
  return hbu_list_char_shift(input);
}

void hbu_pipe_blank_set_input_buffer(hbu_pipe_t pipe, hbu_list_char_t buf) {
  pipe->input = buf;
  pipe->reader = (hbu_pipe_reader_cb_t) &hbu_pipe_read_from_list_char_input;
}

void hbu_pipe_blank_set_output_fstreamout(hbu_pipe_t pipe, hbu_fstreamout_t fstreamout) {
  pipe->output = fstreamout;
  pipe->writer = (hbu_pipe_writer_cb_t) &hbu_fstreamout_write;
}

// Wrapper function for hbu_list_char_append to make it compatible with hbu_pipe_writer_cb_t
void hbu_pipe_write_to_list_char_output(hbe_err_t *hbe_err, hbu_list_char_t output, hb_char_t c) {
  (void) hbe_err;
  hbu_list_char_append(output, c);
}

void hbu_pipe_blank_set_output_buffer(hbu_pipe_t pipe, hbu_list_char_t buf) {
  pipe->output = buf;
  pipe->writer = (hbu_pipe_writer_cb_t) &hbu_pipe_write_to_list_char_output;
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
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @return character or {@link HB_EOD}
 * @throws on read error
 */
hb_eod_char_t hbu_pipe_peek_eoi(hbe_err_t *hbe_err, hbu_pipe_t pipe) {
  // OPTIMISATION: Read directly from buffer if available (no need to unshift later)
  if (pipe->buffer->length) {
    return hbu_list_char_get(pipe->buffer, 0);
  }

  hb_eod_char_t c = HBE_CATCH(_hbu_pipe_read_from_buffer_or_input, pipe);

  if (c != HB_EOD) {
    hbu_list_char_unshift(pipe->buffer, c);
  }

  return c;
}

/**
 * Gets the next character.
 * Will cause an error if it's the end and there is no next character.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @return character
 * @throws on read error or HBE_PARSE_UNEXPECTED_END
 */
hb_char_t hbu_pipe_peek(hbe_err_t *hbe_err, hbu_pipe_t pipe) {
  hb_eod_char_t c = HBE_CATCH(hbu_pipe_peek_eoi, pipe);

  HBE_CATCH(_hbu_pipe_assert_not_eoi, c);

  return c;
}

/**
 * Gets the <i>n</i>th character from current, where <i>n</i> is <code>offset</code>.
 * When <code>offset</code> is 1, the next character is returned (equivalent to {@link hbu_pipe_peek_eoi_offset}).
 * If <code>offset</code> is after the last character, {@link HB_EOD} is returned.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param offset position of character to get
 * @return character or {@link HB_EOD}
 * @throws on read error
 */
hb_eod_char_t hbu_pipe_peek_eoi_offset(hbe_err_t *hbe_err, hbu_pipe_t pipe, size_t offset) {
  HBE_CATCH(_hbu_pipe_ensure_buffer, pipe, offset);

  hb_eod_char_t c = hbu_list_char_get(pipe->buffer, offset - 1);

  return c;
}

/**
 * Gets the <i>n</i>th character from current, where <i>n</i> is <code>offset</code>.
 * When <code>offset</code> is 1, the next character is returned (equivalent to {@link hbu_pipe_peek_offset}).
 * An error will be caused if <code>offset</code> is after the last character.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param offset position of character to get
 * @return character
 * @throws on read error or HBE_PARSE_UNEXPECTED_END
 */
hb_char_t hbu_pipe_peek_offset(hbe_err_t *hbe_err, hbu_pipe_t pipe, size_t offset) {
  hb_eod_char_t c = HBE_CATCH(hbu_pipe_peek_eoi_offset, pipe, offset);

  HBE_CATCH(_hbu_pipe_assert_not_eoi, c);

  return c;
}

/**
 * Checks if the next sequence of characters is the null-terminated character array <code>match</code>.
 * Won't cause an error if insufficient amount of characters left.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param match characters to match
 * @return amount of characters matched, which should be equal to <code>strlen(match)</code>
 * @throws on read error
 */
size_t hbu_pipe_matches(hbe_err_t *hbe_err, hbu_pipe_t pipe, const char *match) {
  size_t matchlen = strlen(match);

  for (size_t i = 0; i < matchlen; i++) {
    hb_eod_char_t c = HBE_CATCH(hbu_pipe_peek_eoi_offset, pipe, i + 1);
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
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param match characters to match case-insensitively
 * @return amount of characters matched, which should be equal to <code>strlen(match)</code>
 * @throws on read error
 */
size_t hbu_pipe_matches_i(hbe_err_t *hbe_err, hbu_pipe_t pipe, const char *match) {
  size_t matchlen = strlen(match);

  for (size_t i = 0; i < matchlen; i++) {
    hb_eod_char_t c = HBE_CATCH(hbu_pipe_peek_eoi_offset, pipe, i + 1);
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
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @return amount of characters matched
 * @throws on read error
 */
size_t hbu_pipe_matches_line_terminator(hbe_err_t *hbe_err, hbu_pipe_t pipe) {
  // Can't chain into HBE_CATCH(...) || HBE_CATCH(...) || ...
  // as HBE_CATCH needs auxiliary statement

  // `\r\n` must be before `\r`
  size_t crlf = HBE_CATCH(hbu_pipe_matches, pipe, "\r\n");
  if (crlf) return crlf;

  size_t cr = HBE_CATCH(hbu_pipe_matches, pipe, "\r");
  if (cr) return cr;

  return HBE_CATCH(hbu_pipe_matches, pipe, "\n");
}

/**
 * Accepts the next character.
 * Will cause an error if already at end.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @return next character
 * @throws on read/write error or HBE_PARSE_UNEXPECTED_END
 */
hb_char_t hbu_pipe_accept(hbe_err_t *hbe_err, hbu_pipe_t pipe) {
  hb_eod_char_t c = HBE_CATCH(_hbu_pipe_read_from_buffer_or_input, pipe);

  HBE_CATCH(_hbu_pipe_assert_not_eoi, c);

  _hbu_pipe_update_pos(pipe, c);

  HBE_CATCH(_hbu_pipe_write_to_output, pipe, c);

  return c;
}

/**
 * Accepts the next <code>count</code> characters.
 * Requires at least <code>count</code> characters remaining.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param count amount of characters
 * @throws on read/write error or HBE_PARSE_UNEXPECTED_END
 */
void hbu_pipe_accept_count(hbe_err_t *hbe_err, hbu_pipe_t pipe, size_t count) {
  for (size_t i = 0; i < count; i++) {
    HBE_CATCH_V(hbu_pipe_accept, pipe);
  }
}

/**
 * Accepts the following character if it is <code>c</code>.
 * Won't match or cause an error if there are no characters remaining.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param c character to match
 * @return 0 if nothing was accepted, 1 otherwise
 * @throws on read/write error
 */
int hbu_pipe_accept_if(hbe_err_t *hbe_err, hbu_pipe_t pipe, hb_char_t c) {
  hb_eod_char_t n = HBE_CATCH(hbu_pipe_peek_eoi, pipe);

  if (n == HB_EOD || n != c) {
    return 0;
  }

  HBE_CATCH(hbu_pipe_accept, pipe);

  return 1;
}

/**
 * Accepts the following characters if they match <code>match</code>.
 * Won't match or cause an error if there are not enough characters remaining.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param match characters to match
 * @return 0 if nothing was accepted, 1 otherwise
 * @throws on read/write error
 */
int hbu_pipe_accept_if_matches(hbe_err_t *hbe_err, hbu_pipe_t pipe, const char *match) {
  size_t matchedlen = HBE_CATCH(hbu_pipe_matches, pipe, match);

  int matched = matchedlen > 0;

  if (matched) {
    HBE_CATCH(hbu_pipe_accept_count, pipe, matchedlen);
  }

  return matched;
}

/**
 * Accepts the following characters if they are either "\r", "\r\n", or "\n".
 * Won't cause an error if insufficient amount of characters left.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @return amount of characters matched
 * @throws on read/write error
 */
size_t hbu_pipe_accept_if_matches_line_terminator(hbe_err_t *hbe_err, hbu_pipe_t pipe) {
  size_t matchedlen = HBE_CATCH(hbu_pipe_matches_line_terminator, pipe);

  if (matchedlen) {
    HBE_CATCH(hbu_pipe_accept_count, pipe, matchedlen);
  }

  return matchedlen;
}

/**
 * Accepts the following character if it satisfies the predicate <code>pred</code>.
 * Won't do anything if already at the end.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param pred predicate
 * @return 0 if nothing was accepted, 1 otherwise
 * @throws on read/write error
 */
int hbu_pipe_accept_if_predicate(hbe_err_t *hbe_err, hbu_pipe_t pipe, hbu_pipe_predicate_t pred) {
  hb_eod_char_t c = HBE_CATCH(hbu_pipe_peek_eoi, pipe);

  if (c == HB_EOD || !(*pred)(c)) {
    return 0;
  }

  HBE_CATCH(hbu_pipe_accept, pipe);

  return 1;
}

/**
 * Accepts every following character until one dissatisfies the predicate <code>pred</code>,
 * or the end is reached.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param pred predicate
 * @return amount of characters accepted
 * @throws on read/write error
 */
size_t hbu_pipe_accept_while_predicate(hbe_err_t *hbe_err, hbu_pipe_t pipe, hbu_pipe_predicate_t pred) {
  size_t count = 0;

  while (1) {
    int matched = HBE_CATCH(hbu_pipe_accept_if_predicate, pipe, pred);
    if (!matched) {
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
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @return skipped character
 * @throws on read error or HBE_PARSE_UNEXPECTED_END
 */
hb_char_t hbu_pipe_skip(hbe_err_t *hbe_err, hbu_pipe_t pipe) {
  hb_eod_char_t c = HBE_CATCH(_hbu_pipe_read_from_buffer_or_input, pipe);

  HBE_CATCH(_hbu_pipe_assert_not_eoi, c);

  _hbu_pipe_update_pos(pipe, c);

  return c;
}

/**
 * Skips over the next <code>amount</code> characters.
 * Requires that the file has at least <code>amount</code> characters remaining.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param amount amount of characters to skip
 * @throws on read error or HBE_PARSE_UNEXPECTED_END
 */
void hbu_pipe_skip_amount(hbe_err_t *hbe_err, hbu_pipe_t pipe, size_t amount) {
  for (size_t i = 0; i < amount; i++) {
    HBE_CATCH_V(hbu_pipe_skip, pipe);
  }
}

/**
 * Skips over every following character until one dissatisfies the predicate <code>pred</code>,
 * or the end is reached.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param pred predicate
 * @return amount of characters skipped
 * @throws on read error
 */
size_t hbu_pipe_skip_while_predicate(hbe_err_t *hbe_err, hbu_pipe_t pipe, hbu_pipe_predicate_t pred) {
  size_t count = 0;

  while (1) {
    hb_eod_char_t c = HBE_CATCH(hbu_pipe_peek_eoi, pipe);

    if (c == HB_EOD || !(*pred)(c)) {
      break;
    }

    HBE_CATCH(hbu_pipe_skip, pipe);
    count++;
  }

  return count;
}

/**
 * Skips over the next sequence of characters if they are <code>match</code>.
 * Requires that the file has at least the length of <code>match</code> characters remaining.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param match sequence of characters to match
 * @return 0 if not matched, 1 otherwise
 * @throws on read error or HBE_PARSE_UNEXPECTED_END
 */
int hbu_pipe_skip_if_matches(hbe_err_t *hbe_err, hbu_pipe_t pipe, const char *match) {
  size_t matchedlen = HBE_CATCH(hbu_pipe_matches, pipe, match);

  int matched = matchedlen > 0;

  if (matched) {
    HBE_CATCH(hbu_pipe_skip_amount, pipe, matchedlen);
  }

  return matched;
}

/**
 * Requires the next character to be <code>c</code>.
 * The matched character is written to output.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param c character to match
 * @throws on read/write error, HBE_PARSE_UNEXPECTED_END, or HBE_PARSE_EXPECTED_NOT_FOUND
 */
void hbu_pipe_require(hbe_err_t *hbe_err, hbu_pipe_t pipe, hb_char_t c) {
  hb_char_t n = HBE_CATCH_V(hbu_pipe_accept, pipe);

  if (c != n) {
    HBU_PIPE_THROW_V(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected `%c` (0x%x), got `%c` (0x%x)", c, c, n, n);
  }
}

/**
 * Requires the next character to be <code>c</code>.
 * The matched character is skipped over and NOT written to output, and also returned.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param c character to match
 * @return matched character
 * @throws on read/write error, HBE_PARSE_UNEXPECTED_END, or HBE_PARSE_EXPECTED_NOT_FOUND
 */
hb_char_t hbu_pipe_require_skip(hbe_err_t *hbe_err, hbu_pipe_t pipe, hb_char_t c) {
  hb_char_t n = HBE_CATCH(hbu_pipe_skip, pipe);

  if (c != n) {
    HBU_PIPE_THROW(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected `%c` (0x%x), got `%c` (0x%x) at %s", c, c, n, n);
  }

  return n;
}

/**
 * Requires the next character to satisfy the predicate <code>pred</code>.
 * The matched character is written to output.
 * If not matched, the error message will describe the expected output using <code>name</code>.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param pred predicate
 * @param name what to output in the error message to describe the requirement
 * @return required character
 * @throws on read/write error, HBE_PARSE_UNEXPECTED_END, or HBE_PARSE_EXPECTED_NOT_FOUND
 */
hb_char_t hbu_pipe_require_predicate(hbe_err_t *hbe_err, hbu_pipe_t pipe, hbu_pipe_predicate_t pred, const char *name) {
  hb_char_t n = HBE_CATCH(hbu_pipe_accept, pipe);

  if (!(*pred)(n)) {
    HBU_PIPE_THROW(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected %s, got `%c` (0x%x)", name, n, n);
  }

  return n;
}

/**
 * Requires the next character to satisfy the predicate <code>pred</code>.
 * The matched character is skipped over and NOT written to output.
 * If not matched, the error message will describe the expected output using <code>name</code>.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param pred predicate
 * @param name what to output in the error message to describe the requirement
 * @return required character
 * @throws on read/write error, HBE_PARSE_UNEXPECTED_END, or HBE_PARSE_EXPECTED_NOT_FOUND
 */
hb_char_t hbu_pipe_require_skip_predicate(hbe_err_t *hbe_err, hbu_pipe_t pipe, hbu_pipe_predicate_t pred, const char *name) {
  hb_char_t n = HBE_CATCH(hbu_pipe_skip, pipe);

  if (!(*pred)(n)) {
    HBU_PIPE_THROW(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected %s, got `%c` (0x%x)", name, n, n);
  }

  return n;
}

/**
 * Requires the next sequence of characters to be equal to <code>match</code>.
 * Matched characters are written to output.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param match sequence of characters to require
 * @throws on read/write error, HBE_PARSE_UNEXPECTED_END, or HBE_PARSE_EXPECTED_NOT_FOUND
 */
void hbu_pipe_require_match(hbe_err_t *hbe_err, hbu_pipe_t pipe, const char *match) {
  int matches = HBE_CATCH_V(hbu_pipe_accept_if_matches, pipe, match);
  if (!matches) {
    HBU_PIPE_THROW_V(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected `%s`", match);
  }
}

/**
 * Requires the next sequence of characters to be equal to <code>match</code>.
 * Matched characters are skipped over and NOT written to output.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param match sequence of characters to require
 * @throws on read/write error, HBE_PARSE_UNEXPECTED_END, or HBE_PARSE_EXPECTED_NOT_FOUND
 */
void hbu_pipe_require_skip_match(hbe_err_t *hbe_err, hbu_pipe_t pipe, const char *match) {
  int matches = HBE_CATCH_V(hbu_pipe_skip_if_matches, pipe, match);
  if (!matches) {
    HBU_PIPE_THROW_V(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected `%s`", match);
  }
}

/**
 * Writes a character.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param c character
 * @throws on write error
 */
void hbu_pipe_write(hbe_err_t *hbe_err, hbu_pipe_t pipe, hb_char_t c) {
  HBE_CATCH_V(_hbu_pipe_write_to_output, pipe, c);
}

/**
 * Writes a character array until a NUL character is reached.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param str null-terminated character array
 * @throws on write error
 */
void hbu_pipe_write_str(hbe_err_t *hbe_err, hbu_pipe_t pipe, hb_char_t *str) {
  hb_char_t c;
  for (size_t i = 0; (c = str[i]); i++) {
    HBE_CATCH_V(_hbu_pipe_write_to_output, pipe, c);
  }
}

/**
 * Writes <code>len</code> characters from the beginning of a character array.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param str character array
 * @param len amount of characters to write
 * @throws on write error
 */
void hbu_pipe_write_str_len(hbe_err_t *hbe_err, hbu_pipe_t pipe, hb_char_t *str, size_t len) {
  for (size_t i = 0; i < len; i++) {
    HBE_CATCH_V(_hbu_pipe_write_to_output, pipe, str[i]);
  }
}

/**
 * Writes a buffer.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param buffer buffer
 * @throws on write error
 */
void hbu_pipe_write_buffer(hbe_err_t *hbe_err, hbu_pipe_t pipe, hbu_list_char_t buffer) {
  for (size_t i = 0; i < buffer->length; i++) {
    HBE_CATCH_V(_hbu_pipe_write_to_output, pipe, hbu_list_char_get(buffer, i));
  }
}

/**
 * Writes a character from its Unicode code point in UTF-8 encoding, which may be multiple bytes.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param code_point Unicode code point
 * @return amount of bytes written (0 if invalid code point)
 * @throws on write error
 */
// Logic copied from https://gist.github.com/MightyPork/52eda3e5677b4b03524e40c9f0ab1da5
int hbu_pipe_write_unicode(hbe_err_t *hbe_err, hbu_pipe_t pipe, uint32_t code_point) {
   if (code_point <= 0x7F) {
    // Plain ASCII
    HBE_CATCH(hbu_pipe_write, pipe, (hb_char_t) code_point);
    return 1;
  }

  if (code_point <= 0x07FF) {
    // 2-byte unicode
    HBE_CATCH(hbu_pipe_write, pipe, (hb_char_t) (((code_point >> 6) & 0x1F) | 0xC0));
    HBE_CATCH(hbu_pipe_write, pipe, (hb_char_t) (((code_point >> 0) & 0x3F) | 0x80));
    return 2;
  }

  if (code_point <= 0xFFFF) {
    // 3-byte unicode
    HBE_CATCH(hbu_pipe_write, pipe, (hb_char_t) (((code_point >> 12) & 0x0F) | 0xE0));
    HBE_CATCH(hbu_pipe_write, pipe, (hb_char_t) (((code_point >>  6) & 0x3F) | 0x80));
    HBE_CATCH(hbu_pipe_write, pipe, (hb_char_t) (((code_point >>  0) & 0x3F) | 0x80));
    return 3;
  }

  if (code_point <= 0x10FFFF) {
    // 4-byte unicode
    HBE_CATCH(hbu_pipe_write, pipe, (hb_char_t) (((code_point >> 18) & 0x07) | 0xF0));
    HBE_CATCH(hbu_pipe_write, pipe, (hb_char_t) (((code_point >> 12) & 0x3F) | 0x80));
    HBE_CATCH(hbu_pipe_write, pipe, (hb_char_t) (((code_point >>  6) & 0x3F) | 0x80));
    HBE_CATCH(hbu_pipe_write, pipe, (hb_char_t) (((code_point >>  0) & 0x3F) | 0x80));
    return 4;
  }

  return 0;
}
