#ifndef _HDR_HYPERBUILD_RULE_VOIDTAGS
#define _HDR_HYPERBUILD_RULE_VOIDTAGS

#include "../ext/khash.h"

const char *hb_r_voidtags_raw[] = {
  "area",
  "base",
  "br",
  "col",
  "embed",
  "hr",
  "img",
  "input",
  "keygen",
  "link",
  "meta",
  "param",
  "source",
  "track",
  "wbr",
};

KHASH_SET_INIT_STR(hb_r_voidtags)

static khash_t(hb_r_voidtags) *hb_r_voidtags_h;

static void hb_r_voidtags_init() {
  hb_r_voidtags_h = kh_init(hb_r_voidtags);

  int rv;

  for (int i = 0; i < sizeof(hb_r_voidtags_raw) / sizeof(hb_r_voidtags_raw[0]); i++) {
    kh_put(hb_r_voidtags, hb_r_voidtags_h, hb_r_voidtags_raw[i], &rv);
  }
}

int hb_r_voidtags_check(char *tag) {
  khint_t k = kh_get(hb_r_voidtags, hb_r_voidtags_h, tag);
  return k != kh_end(hb_r_voidtags_h);
}

void hb_r_voidtags_init() {
  hb_r_voidtags_init();
}

#endif // _HDR_HYPERBUILD_RULE_VOIDTAGS
