#ifndef _HDR_HYPERBUILD_RULE_INLINETAGS
#define _HDR_HYPERBUILD_RULE_INLINETAGS

#include "../ext/khash.h"

const char *hb_r_inlinetags_raw[] = {
  "a",
  "abbr",
  "b",
  "bdi",
  "bdo",
  "br",
  "cite",
  "code",
  "data",
  "dfn",
  "em",
  "i",
  "kbd",
  "mark",
  "q",
  "rp",
  "rt",
  "rtc",
  "ruby",
  "s",
  "samp",
  "small",
  "span",
  "strong",
  "sub",
  "sup",
  "time",
  "tt",
  "u",
  "var",
  "wbr",
};

KHASH_SET_INIT_STR(hb_r_inlinetags)

static khash_t(hb_r_inlinetags) *hb_r_inlinetags_h;

static void hb_r_inlinetags_init() {
  hb_r_inlinetags_h = kh_init(hb_r_inlinetags);

  int rv;

  for (int i = 0; i < sizeof(hb_r_inlinetags_raw) / sizeof(hb_r_inlinetags_raw[0]); i++) {
    kh_put(hb_r_inlinetags, hb_r_inlinetags_h, hb_r_inlinetags_raw[i], &rv);
  }
}

int hb_r_inlinetags_check(char *tag) {
  khint_t k = kh_get(hb_r_inlinetags, hb_r_inlinetags_h, tag);
  return k != kh_end(hb_r_inlinetags_h);
}

void hb_r_inlinetags_init() {
  hb_r_inlinetags_init();
}

#endif // _HDR_HYPERBUILD_RULE_INLINETAGS
