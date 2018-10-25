#include <string.h>
#include "../char/char.c"
#include "./__base__.c"

HBU_LIST(hbu_list_char, hb_char_t, SIZEOF_CHAR, hb_eod_char_t, HB_EOD);

int hbu_list_char_compare(hbu_list_char_t a, hbu_list_char_t b) {
  // All buffers have NULL-terminated underlying char arrays
  return strcmp((char *) hbu_list_char_underlying(a), (char *) hbu_list_char_underlying(b));
}

int hbu_list_char_compare_lit(hbu_list_char_t a, const char *b) {
  return strcmp((char *) hbu_list_char_underlying(a), b);
}
