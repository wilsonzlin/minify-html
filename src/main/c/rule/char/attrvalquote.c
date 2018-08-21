#include "./c0.c"

static nh_set_int32_t hbr_attrvalquote_set;

void hbr_attrvalquote_init(void) {
  hbr_attrvalquote_set = nh_set_int32_create();
  nh_set_int32_add(hbr_attrvalquote_set, '\'');
  nh_set_int32_add(hbr_attrvalquote_set, '"');
}

int hbr_attrvalquote_check(hb_char_t c) {
  return nh_set_int32_has(hbr_attrvalquote_set, c);
}
