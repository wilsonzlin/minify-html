#ifndef _HDR_HYPERBUILD_UTIL_BUFFER
#define _HDR_HYPERBUILD_UTIL_BUFFER

#include <string.h>
#include "hbchar.h"
#include "./list.h"

HBU_LIST(hbu_buffer, hb_char_t, SIZEOF_CHAR, hb_eod_char_t, HB_EOD);

int hbu_buffer_compare(hbu_buffer_t a, hbu_buffer_t b) {
  // All buffers have NULL-terminated underlying char arrays
  return strcmp((char *) a->data, (char *) b->data);
}

int hbu_buffer_compare_lit(hbu_buffer_t a, const char *b) {
  return strcmp((char *) a->data, b);
}

#endif
