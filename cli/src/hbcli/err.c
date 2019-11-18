#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void hbcli_err(char *format, ...) {
  va_list args;
  va_start(args, format);

  bool is_tty = isatty(fileno(stdout));

  if (is_tty) {
    fprintf(stderr, "\x1B[31m\x1B[1m");
  }
  vfprintf(stderr, format, args);
  if (is_tty) {
    fprintf(stderr, "\x1B[0m");
  }

  va_end(args);

  fprintf(stderr, "\n");
  exit(1);
}
