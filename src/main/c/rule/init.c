#ifndef _HDR_HYPERBUILD_RULE_INIT
#define _HDR_HYPERBUILD_RULE_INIT

#include "./char/attrname.c"
#include "./char/attrvalquote.c"
#include "./char/c0.c"
#include "./char/digit.c"
#include "./char/hex.c"
#include "./char/lcalpha.c"
#include "./char/tagname.c"
#include "./char/ucalpha.c"
#include "./char/unquotedattrval.c"
#include "./char/whitespace.c"

#include "./entity/entityrefs.c"

#include "./relation/blacklistchildren.c"
#include "./relation/blacklistparents.c"
#include "./relation/whitelistchildren.c"
#include "./relation/whitelistparents.c"

#include "./tag/contentfirsttags.c"
#include "./tag/contenttags.c"
#include "./tag/formattingtags.c"
#include "./tag/headingtags.c"
#include "./tag/layouttags.c"
#include "./tag/mediatags.c"
#include "./tag/sectioningtags.c"
#include "./tag/specifictags.c"
#include "./tag/tags.c"
#include "./tag/voidtags.c"
#include "./tag/wsstags.c"
#include "./tag/svgtags.c"

void hbr_init(void) {
  // Core
  hbr_c0_init();
  hbr_digit_init();
  hbr_hex_init();
  hbr_ucalpha_init();
  hbr_lcalpha_init();
  hbr_whitespace_init();

  // Identifiers
  hbr_tagname_init();
  hbr_attrname_init();

  // Values
  hbr_attrvalquote_init();
  hbr_unquotedattrval_init();
  hbr_entityrefs_init();

  // Specification tag categories
  hbr_headingtags_init();
  hbr_mediatags_init();
  hbr_sectioningtags_init();

  hbr_voidtags_init();
  hbr_wsstags_init();

  hbr_svgtags_init();
  hbr_tags_init();

  // Hyperbuild tag categories
  hbr_contentfirsttags_init();
  hbr_contenttags_init();
  hbr_formattingtags_init();
  hbr_layouttags_init();
  hbr_specifictags_init();

  // Relations
  hbr_whitelistparents_init();
  hbr_blacklistparents_init();
  hbr_whitelistchildren_init();
  hbr_blacklistchildren_init();
}

#endif // _HDR_HYPERBUILD_RULE_INIT
