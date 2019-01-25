#include "./htmltags.c"
#include "./svgtags.c"

void hb_rule_tags_init(void) {
}

int hb_rule_tags_check(hb_proc_char_t *tag) {
  return hb_rule_htmltags_check(tag) ||
         hb_rule_svgtags_check(tag);
}
