#ifndef _HDR_HYPERBUILD_RULE_TAGS
#define _HDR_HYPERBUILD_RULE_TAGS

#include "../../ext/nicehash/set/str.h"
#include "../../util/hbchar.h"

#include "./htmltags.c"
#include "./svgtags.c"

void hbr_tags_init(void) {
}

int hbr_tags_check(hb_char_t *tag) {
  return hbr_htmltags_check(tag) ||
         hbr_svgtags_check(tag);
}

#endif // _HDR_HYPERBUILD_RULE_TAGS
