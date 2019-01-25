#pragma once

#include <stdbool.h>
#include <string.h>
#include "../../rule/char/ucalpha.c"
#include "../char/char.c"
#include "../execution/error.c"
#include "../list/char.c"
#include "../fstream/fstreamin.c"
#include "../fstream/fstreamout.c"

// Use macro to prevent having to allocate (and therefore free/manage) memory
#define HB_PROC_FORMAT_WITH_POS(fn, a, format, ...) fn(a, format " at %s [line %d, column %d]", __VA_ARGS__, proc->name, proc->line, proc->column);

/**
 * Creates an error using a message with the current position appended.
 *
 * @param proc proc
 * @param errcode error code
 * @param reason message
 * @return error
 */
hbe_err_t hb_proc_error(hb_proc_t* proc, hb_error_t errcode, const char *reason, ...) {
  va_list args;
  va_start(args, reason);

  char *msg = calloc(HB_PROC_MAX_ERR_MSG_LEN + 1, SIZEOF_CHAR);
  vsnprintf(msg, HB_PROC_MAX_ERR_MSG_LEN, reason, args);

  va_end(args);

  hbe_err_t err = HBU_FN_FORMAT_WITH_POS(hbe_err_create, errcode, "%s", msg);
  free(msg);

  return err;
}

/**
 * Reads from input.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param proc proc
 * @throws on read error
 */
static hb_eod_char_t _hb_proc_read_from_input(hbe_err_t *hbe_err, hb_proc_t* proc) {
  hb_eod_char_t c = HBE_CATCH((*proc->reader), proc->input);
  if (c == HB_EOD) {
    proc->EOI = 1;
  }
  return c;
}

/**
 * Ensures the buffer has loaded at least the next <code>offset</code> characters, or the remaining
 * characters from input if there are less than <code>offset</code> characters left.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param proc proc
 * @throws on read error
 */
static void _hb_proc_ensure_buffer(hbe_err_t *hbe_err, hb_proc_t* proc, size_t offset) {
  if (proc->EOI) {
    return;
  }

  size_t current = proc->buffer->length;

  if (offset <= current) {
    return;
  }

  size_t gap = offset - current;

  for (size_t i = 0; i < gap; i++) {
    hb_eod_char_t c = HBE_CATCH_V(_hb_proc_read_from_input, proc);
    if (c == HB_EOD) {
      // EOI flag already set by _hb_proc_read_from_input
      return;
    }
    hb_list_char_append(proc->buffer, c);
  }
}

/**
 * Gets the next character, whether it's by shifting the buffer or reading from input.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param proc proc
 * @throws on read error
 */
static hb_eod_char_t _hb_proc_read_from_buffer_or_input(hbe_err_t *hbe_err, hb_proc_t* proc) {
  if (proc->EOI) {
    return HB_EOD;
  }

  if (proc->buffer->length) {
    return hb_list_char_shift(proc->buffer);
  }

  return HBE_CATCH(_hb_proc_read_from_input, proc);
}

static void _hb_proc_update_pos(hb_proc_t* proc, hb_proc_char_t c) {
  switch (c) {
  case '\r':
    proc->CR = true;
    proc->line++;
    proc->column = 0;
    break;

  case '\n':
    if (!proc->CR) {
      proc->line++;
      proc->column = 0;
    } else {
      proc->CR = false;
    }
    break;

  default:
    proc->column++;
    proc->CR = false;
  }
}

/**
 * Ensures that the provided character is not @{link HB_EOD}, and causes an error otherwise.
 *
 * @param c character to test for <code>HB_EOD</code>
 * @throws HBE_PARSE_UNEXPECTED_END if <code>c</code> is <code>HB_EOD</code>
 */
static void _hb_proc_assert_not_eoi(hbe_err_t *hbe_err, hb_eod_char_t c) {
  if (c == HB_EOD) {
    HBE_THROW_V(HBE_PARSE_UNEXPECTED_END, "Unexpected end of input");
  }
}

/**
 * Writes a character to the redirect, if enabled, otherwise output, of a proc,
 * unless the output is masked.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param proc proc
 * @param c character to write
 * @return a freshly-created proc
 * @throws on write error
 */
static void _hb_proc_write_to_output(hbe_err_t *hbe_err, hb_proc_t* proc, hb_proc_char_t c) {
  if (!proc->mask) {
    hb_list_char_t redirect = proc->redirect;
    if (redirect != NULL) {
      hb_list_char_append(redirect, c);
    } else {
      HBE_CATCH_V((*proc->writer), proc->output, c);
    }
  }
}

/*
 *
 * INSTANCE MANAGEMENT FUNCTIONS
 *
 */

/**
 * Allocates memory for a proc, and creates one with provided arguments.
 *
 * @param input input
 * @param reader reader
 * @param name name
 * @param output output
 * @param writer writer
 * @return a freshly-created proc
 */
hb_proc_t* hb_proc_create_blank(char *name) {
  hb_proc_t* proc = calloc(1, sizeof(hb_proc_t));

  proc->name = name;

  proc->input = NULL;
  proc->reader = NULL;
  proc->EOI = false;

  proc->line = 1;
  proc->column = 0;
  proc->CR = false;

  proc->output = NULL;
  proc->writer = NULL;
  proc->buffer = nh_list_ucp_create();
  proc->mask = false;
  proc->redirect = NULL;

  return proc;
}

/**
 * Frees all memory associated with a proc.
 *
 * @param proc proc
 */
void hb_proc_destroy(hb_proc_t* proc) {
  nh_list_ucp_destroy(proc->buffer);
  free(proc);
}

/**
 * Enables or disables the output mask.
 * When the output mask is enabled, all writes are simply discarded and not actually written to output.
 *
 * @param proc proc
 * @param mask 1 to enable, 0 to disable
 * @return previous state
 */
int hb_proc_toggle_output_mask(hb_proc_t* proc, int mask) {
  int current = proc->mask;
  proc->mask = mask;
  return current;
}

/**
 * Enables or disables the output redirect.
 * When the output redirect is enabled, all writes are written to a buffer instead of the output.
 *
 * @param proc proc
 * @param redirect buffer to redirect writes to, or NULL to disable
 */
void hb_proc_set_redirect(hb_proc_t* proc, hb_list_char_t redirect) {
  proc->redirect = redirect;
}

void hb_proc_blank_set_input_fstreamin(hb_proc_t* proc, hbu_fstreamin_t fstreamin) {
  proc->input = fstreamin;
  proc->reader = (hb_proc_reader_cb_t) &hbu_fstreamin_read;
}

// Wrapper function for hb_list_char_shift to make it compatible with hb_proc_reader_cb_t
static hb_eod_char_t hb_proc_read_from_list_char_input(hbe_err_t *hbe_err, hb_list_char_t input) {
  (void) hbe_err;
  return hb_list_char_shift(input);
}

void hb_proc_blank_set_input_buffer(hb_proc_t* proc, hb_list_char_t buf) {
  proc->input = buf;
  proc->reader = (hb_proc_reader_cb_t) &hb_proc_read_from_list_char_input;
}

static void hb_proc_blank_set_output_fstreamout(hb_proc_t* proc, hbu_fstreamout_t fstreamout) {
  proc->output = fstreamout;
  proc->writer = (hb_proc_writer_cb_t) &hbu_fstreamout_write;
}

// Wrapper function for hb_list_char_append to make it compatible with hb_proc_writer_cb_t
void hb_proc_write_to_list_char_output(hbe_err_t *hbe_err, hb_list_char_t output, hb_proc_char_t c) {
  (void) hbe_err;
  hb_list_char_append(output, c);
}

void hb_proc_blank_set_output_buffer(hb_proc_t* proc, hb_list_char_t buf) {
  proc->output = buf;
  proc->writer = (hb_proc_writer_cb_t) &hb_proc_write_to_list_char_output;
}
