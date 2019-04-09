#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <hb/rune.h>
#include <hb/cfg.h>
#include <hb/err.h>
#include <setjmp.h>

// Result of processing.
typedef struct {
  // The error code, which could be HB_ERR_OK if no errors occurred (i.e. processing completed successfully).
  hb_err code;
  // Error message if an error occurred.
  char* msg;
  // Whether or not msg should be freed on destruction.
  bool custom;
  // The value of src_next at the time of error.
  size_t pos;
} hb_proc_result;

// A view represents a substring of the source. Faster, easier, safer, and more efficient than making a copy.
// If the end is before or at the start, it's invalid, like NaN. Can be used for special meaning.
typedef struct {
    // Inclusive.
    size_t start;
    // Exclusive.
    size_t end;
} hb_proc_view;

// Processing state of a file. Most fields are used internally and set during processing.
// Single use only; create one per processing.
typedef struct {
  // Settings for this run.
  hb_cfg* cfg;
  // Name of the source, usually the file name or something descriptive and unique such as stdin.
  char* name;
  // This will be set just before starting to process so that when an error occurs, the processor will jump back to where this was set.
  // This is known as a long jump and saves having to check if an error occurred at every stage of processing.
  jmp_buf start;

  // Source data, represented as an array of bytes (see hb_rune).
  // To avoid having repeated checks and a dedicated marker/struct field for EOF, the src array will terminate with HB_EOF, an invalid Unicode byte.
  hb_rune* src;
  // Length of the source data.
  size_t src_len;
  // Offset of the next unconsumed character.
  // This means that when src_next == src_len, there are no more unconsumed characters, the end has been reached, and the input has been processed.
  size_t src_next;

  // Where to write the output.
  hb_rune* out;
  // Offset of the next unwritten space.
  size_t out_next;
  // Result of processing, set on completion or error.
  hb_proc_result result;
} hb_proc;

// Memory to allocate for a custom error message.
#define HB_PROC_ERROR_CUSTOM_SIZE 512

// Signature for a predicate function that returns true or false given a character.
typedef bool hb_proc_pred(hb_rune);

// Method declarations for implementations in source files under hb/proc, sorted by declaration order, grouped by file name in alphabetical order.

hb_rune hb_proc_accept(hb_proc* proc);
void hb_proc_accept_count(hb_proc* proc, size_t count);
size_t hb_proc_accept_if(hb_proc* proc, hb_rune c);
size_t hb_proc_accept_if_matches(hb_proc* proc, char const* match);
size_t hb_proc_accept_if_matches_line_terminator(hb_proc* proc);
size_t hb_proc_accept_if_predicate(hb_proc* proc, hb_proc_pred* pred);
size_t hb_proc_accept_while_predicate(hb_proc* proc, hb_proc_pred* pred);

void hb_proc_bounds_assert_not_eof(hb_proc* proc, hb_eof_rune c);
bool hb_proc_bounds_check_offset(hb_proc* proc, size_t offset);
void hb_proc_bounds_assert_offset(hb_proc* proc, size_t offset);

#define hb_proc_matches(proc, match) hb_proc_matches_len(proc, match, sizeof(match))
size_t hb_proc_matches_len(hb_proc* proc, char const* match, size_t match_len);
#define hb_proc_matches_i(proc, match) hb_proc_matches_len_i(proc, match, sizeof(match))
size_t hb_proc_matches_len_i(hb_proc* proc, char const* match, size_t match_len);
size_t hb_proc_matches_line_terminator(hb_proc* proc);

#define hb_proc_error(proc, code, msg) hb_proc_error_pos(proc, code, proc->src_next, msg)
void hb_proc_error_pos(hb_proc* proc, hb_err code, size_t pos, char const* msg);
#define hb_proc_error_custom(proc, code, format, ...) hb_proc_error_custom_pos(proc, code, proc->src_next, format, __VA_ARGS__)
void hb_proc_error_custom_pos(hb_proc* proc, hb_err code, size_t pos, char const* format, ...);

hb_eof_rune hb_proc_peek_eof(hb_proc* proc);
hb_rune hb_proc_peek(hb_proc* proc);
hb_eof_rune hb_proc_peek_eof_offset(hb_proc* proc, size_t offset);
hb_rune hb_proc_peek_offset(hb_proc* proc, size_t offset);

void hb_proc_require(hb_proc* proc, hb_rune c);
hb_rune hb_proc_require_skip(hb_proc* proc, hb_rune c);
hb_rune hb_proc_require_predicate(hb_proc* proc, hb_proc_pred* pred, char const* name);
hb_rune hb_proc_require_skip_predicate(hb_proc* proc, hb_proc_pred* pred, char const* name);
void hb_proc_require_match(hb_proc* proc, char const* match);
void hb_proc_require_skip_match(hb_proc* proc, char const* match);

hb_rune hb_proc_skip(hb_proc* proc);
void hb_proc_skip_amount(hb_proc* proc, size_t amount);
size_t hb_proc_skip_if(hb_proc* proc, hb_rune c);
size_t hb_proc_skip_while_predicate(hb_proc* proc, hb_proc_pred* pred);
size_t hb_proc_skip_if_matches(hb_proc* proc, char const* match);

// Check that a view is invalid, i.e. the end is before or at the start.
bool hb_proc_view_invalid(hb_proc_view view);
void hb_proc_view_mark_start_with_prev(hb_proc_view* view, hb_proc* proc);
void hb_proc_view_mark_start_with_next(hb_proc_view* view, hb_proc* proc);
void hb_proc_view_mark_end_with_prev(hb_proc_view* view, hb_proc* proc);
void hb_proc_view_mark_end_with_next(hb_proc_view* view, hb_proc* proc);

void hb_proc_write(hb_proc* proc, hb_rune c);
