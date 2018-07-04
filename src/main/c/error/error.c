#ifndef _HDR_HYPERBUILD_ERROR
#define _HDR_HYPERBUILD_ERROR

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

void hbe_fatal(char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "[FATAL] hyperbuild encountered an error:\n");
  vfprintf(stderr, fmt, args);
  printf("\n");
  va_end(args);
  exit(EXIT_FAILURE);
}

// TODO errcode enum used as exit statuses

#endif // _HDR_HYPERBUILD_ERROR
