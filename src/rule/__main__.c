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

#include "./entity/entity_references.c"

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
#include "./tag/voidtags.c"
#include "./tag/wsstags.c"
#include "./tag/htmltags.c"
#include "./tag/svgtags.c"
#include "./tag/tags.c"

void hb_rule_init(void) {
  // Core
  hb_rule_c0_init();
  hb_rule_digit_init();
  hb_rule_hex_init();
  hb_rule_ucalpha_init();
  hb_rule_lcalpha_init();
  hb_rule_whitespace_init();

  // Identifiers
  hb_rule_tagname_init();
  hb_rule_attrname_init();

  // Values
  hb_rule_attrvalquote_init();
  hb_rule_unquotedattrval_init();
  hb_rule_entity_references_init();

  // Specification tag categories
  hb_rule_headingtags_init();
  hb_rule_mediatags_init();
  hb_rule_sectioningtags_init();

  hb_rule_voidtags_init();
  hb_rule_wsstags_init();

  hb_rule_htmltags_init();
  hb_rule_svgtags_init();
  hb_rule_tags_init();

  // Hyperbuild tag categories
  hb_rule_contentfirsttags_init();
  hb_rule_contenttags_init();
  hb_rule_formattingtags_init();
  hb_rule_layouttags_init();
  hb_rule_specifictags_init();

  // Relations
  hb_rule_whitelistparents_init();
  hb_rule_blacklistparents_init();
  hb_rule_whitelistchildren_init();
  hb_rule_blacklistchildren_init();
}

#endif // _HDR_HYPERBUILD_RULE_INIT
