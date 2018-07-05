#ifndef _HDR_HYPERBUILD_RULE_INIT
#define _HDR_HYPERBUILD_RULE_INIT

#include "./char/attrname.c"
#include "./char/attrvalquote.c"
#include "./char/c0.c"
#include "./char/digit.c"
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

void hb_r_init(void) {
  // Core
  hb_r_c0_init();
  hb_r_digit_init();
  hb_r_ucalpha_init();
  hb_r_lcalpha_init();
  hb_r_whitespace_init();

  // Identifiers
  hb_r_tagname_init();
  hb_r_attrname_init();

  // Values
  hb_r_attrvalquote_init();
  hb_r_unquotedattrval_init();
  hb_r_entityrefs_init();

  // Specification tag categories
  hb_r_headingtags_init();
  hb_r_mediatags_init();
  hb_r_sectioningtags_init();
  
  hb_r_voidtags_init();
  hb_r_wsstags_init();

  hb_r_tags_init();

  // Hyperbuild tag categories
  hb_r_contentfirsttags_init();
  hb_r_contenttags_init();
  hb_r_formattingtags_init();
  hb_r_layouttags_init();
  hb_r_specifictags_init();

  // Relations
  hb_r_whitelistparents_init();
  hb_r_blacklistparents_init();
  hb_r_whitelistchildren_init();
  hb_r_blacklistchildren_init();
}

#endif // _HDR_HYPERBUILD_RULE_INIT
