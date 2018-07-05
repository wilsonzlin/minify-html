#ifndef _HDR_HYPERBUILD_RULE_CONTENTTAGS
#define _HDR_HYPERBUILD_RULE_CONTENTTAGS

#include "../../ext/nicehash/set/str.h"
#include "../../util/hbchar.h"

static nh_set_str_t hb_r_contenttags_set;

void hb_r_contenttags_add_elems(nh_set_str_t set) {
  nh_set_str_add(set, "address");
  nh_set_str_add(set, "audio");
  nh_set_str_add(set, "button");
  nh_set_str_add(set, "canvas");
  nh_set_str_add(set, "caption");
  nh_set_str_add(set, "figcaption");
  nh_set_str_add(set, "h1");
  nh_set_str_add(set, "h2");
  nh_set_str_add(set, "h3");
  nh_set_str_add(set, "h4");
  nh_set_str_add(set, "h5");
  nh_set_str_add(set, "h6");
  nh_set_str_add(set, "legend");
  nh_set_str_add(set, "meter");
  nh_set_str_add(set, "object");
  nh_set_str_add(set, "option");
  nh_set_str_add(set, "p");
  nh_set_str_add(set, "summary"); // Can also contain a heading
  nh_set_str_add(set, "textarea");
  nh_set_str_add(set, "video");
}

void hb_r_contenttags_init(void) {
  hb_r_contenttags_set = nh_set_str_create();
  hb_r_contenttags_add_elems(hb_r_contenttags_set);
}

int hb_r_contenttags_check(hb_char_t *tag) {
  return nh_set_str_has(hb_r_contenttags_set, (char *) tag);
}

#endif // _HDR_HYPERBUILD_RULE_CONTENTTAGS
