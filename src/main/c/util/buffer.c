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

// Logic copied from https://gist.github.com/MightyPork/52eda3e5677b4b03524e40c9f0ab1da5
int hbu_buffer_append_unicode(hbu_buffer_t buffer, uint32_t code_point) {
   if (code_point <= 0x7F) {
    // Plain ASCII
    hbu_buffer_append(buffer, (hb_char_t) code_point);
    return 1;
  }

  if (code_point <= 0x07FF) {
    // 2-byte unicode
    hbu_buffer_append(buffer, (hb_char_t) (((code_point >> 6) & 0x1F) | 0xC0));
    hbu_buffer_append(buffer, (hb_char_t) (((code_point >> 0) & 0x3F) | 0x80));
    return 2;
  }

  if (code_point <= 0xFFFF) {
    // 3-byte unicode
    hbu_buffer_append(buffer, (hb_char_t) (((code_point >> 12) & 0x0F) | 0xE0));
    hbu_buffer_append(buffer, (hb_char_t) (((code_point >>  6) & 0x3F) | 0x80));
    hbu_buffer_append(buffer, (hb_char_t) (((code_point >>  0) & 0x3F) | 0x80));
    return 3;
  }

  if (code_point <= 0x10FFFF) {
    // 4-byte unicode
    hbu_buffer_append(buffer, (hb_char_t) (((code_point >> 18) & 0x07) | 0xF0));
    hbu_buffer_append(buffer, (hb_char_t) (((code_point >> 12) & 0x3F) | 0x80));
    hbu_buffer_append(buffer, (hb_char_t) (((code_point >>  6) & 0x3F) | 0x80));
    hbu_buffer_append(buffer, (hb_char_t) (((code_point >>  0) & 0x3F) | 0x80));
    return 4;
  }

  return 0;
}

#endif
