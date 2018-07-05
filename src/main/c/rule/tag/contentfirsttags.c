#ifndef _HDR_HYPERBUILD_RULE_CONTENTFIRSTTAGS
#define _HDR_HYPERBUILD_RULE_CONTENTFIRSTTAGS

#include "../../ext/nicehash/set/str.h"
#include "../../util/hbchar.h"

static nh_set_str_t hb_r_contentfirsttags_set;

void hb_r_contentfirsttags_add_elems(nh_set_str_t set) {
  nh_set_str_add(set, "dd");
  nh_set_str_add(set, "details");
  nh_set_str_add(set, "dt");
  nh_set_str_add(set, "iframe");
  nh_set_str_add(set, "label");
  nh_set_str_add(set, "li");
  nh_set_str_add(set, "noscript");
  nh_set_str_add(set, "output");
  nh_set_str_add(set, "progress");
  nh_set_str_add(set, "slot");
  nh_set_str_add(set, "td");
  nh_set_str_add(set, "template");
  nh_set_str_add(set, "th");
}

void hb_r_contentfirsttags_init(void) {
  hb_r_contentfirsttags_set = nh_set_str_create();
  hb_r_contentfirsttags_add_elems(hb_r_contentfirsttags_set);
}

int hb_r_contentfirsttags_check(hb_char_t *tag) {
  return nh_set_str_has(hb_r_contentfirsttags_set, (char *) tag);
}

#endif // _HDR_HYPERBUILD_RULE_CONTENTFIRSTTAGS
