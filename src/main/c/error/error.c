#ifndef _HDR_HYPERBUILD_ERROR
#define _HDR_HYPERBUILD_ERROR

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#define PC_RED   "\x1B[31m"
#define PC_GRN   "\x1B[32m"
#define PC_YEL   "\x1B[33m"
#define PC_BLU   "\x1B[34m"
#define PC_MAG   "\x1B[35m"
#define PC_CYN   "\x1B[36m"
#define PC_WHT   "\x1B[37m"
#define PC_RESET "\x1B[0m"
#define PC_BOLD  "\e[1m"

#define _HBE_KV_LOGLINE_LEFTWIDTH "35"

typedef enum hbe_errcode {
  HBE_INTERR_PEEK_OFFSET_GEQ_ZERO = 1,

  HBE_CLI_TOO_MANY_OPTIONS = 17,
  HBE_CLI_INVALID_TAG_SET,
  HBE_CLI_INVALID_TAG,
  HBE_CLI_INVALID_SUPPRESSABLE_ERROR,

  HBE_IO_FOPEN_FAIL = 33,
  HBE_IO_FCLOSE_FAIL,
  HBE_IO_FREAD_FAIL,
  HBE_IO_FWRITE_FAIL,

  HBE_PARSE_MALFORMED_ENTITY = 65,
  HBE_PARSE_INVALID_ENTITY,
  HBE_PARSE_NONSTANDARD_TAG,
  HBE_PARSE_UCASE_TAG,
  HBE_PARSE_UCASE_ATTR,
  HBE_PARSE_UNQUOTED_ATTR,
  HBE_PARSE_ILLEGAL_CHILD,
  HBE_PARSE_UNCLOSED_TAG,

  HBE_PARSE_UNEXPECTED_END,
  HBE_PARSE_EXPECTED_NOT_FOUND,
} hbe_errcode_t;

static char *_hbe_fatal_autodelete_file = NULL;
static int _hbe_info_enabled = 0;

void hbe_fatal_set_autodelete(char *path) {
  _hbe_fatal_autodelete_file = path;
}

void hbe_debug(char *fmt, ...) {
  if (_hbe_info_enabled) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "[DEBUG] ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
  }
}

void hbe_info_toggle(int log_info) {
  _hbe_info_enabled = log_info;
}

void hbe_info(char *fmt, ...) {
  if (_hbe_info_enabled) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, PC_MAG "[INFO] " PC_RESET);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
  }
}

void hbe_info_kv_boolean(char *name, int state) {
  const char *color = state ? (PC_BOLD PC_GRN) : PC_MAG;
  const char *label = state ? "ON" : "OFF";

  hbe_info("%-" _HBE_KV_LOGLINE_LEFTWIDTH "s%s%s" PC_RESET, name, color, label);
}

void hbe_info_kv_string(char *name, char *value) {
  hbe_info("%-" _HBE_KV_LOGLINE_LEFTWIDTH "s" PC_BOLD "%s" PC_RESET, name, value);
}

void hbe_warn(char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, PC_BOLD PC_YEL "[WARN] " PC_RESET);
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  va_end(args);
}

void hbe_fatal(hbe_errcode_t errcode, char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, PC_BOLD PC_RED "[FATAL]" PC_RESET PC_RED " Error %d: " PC_RESET, errcode);
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  va_end(args);

  if (_hbe_fatal_autodelete_file != NULL) {
    if (unlink(_hbe_fatal_autodelete_file)) {
      hbe_warn("Failed to delete file %s with error %d", _hbe_fatal_autodelete_file, errno);
    } else {
      hbe_info("%s has been deleted", _hbe_fatal_autodelete_file);
    }
  }

  // NOTE: $errcode must be less than 256 (see man exit(3))
  exit(errcode);
}

#endif // _HDR_HYPERBUILD_ERROR
