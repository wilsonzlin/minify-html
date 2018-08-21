#include "./htmltags.c"
#include "./svgtags.c"

void hbr_tags_init(void) {
}

int hbr_tags_check(hb_char_t *tag) {
  return hbr_htmltags_check(tag) ||
         hbr_svgtags_check(tag);
}
