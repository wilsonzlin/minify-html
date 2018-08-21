#include "./lcalpha.c"
#include "./ucalpha.c"
#include "./digit.c"

static nh_set_int32_t hbr_tagname_set;

void hbr_tagname_init(void) {
  hbr_tagname_set = nh_set_int32_create();
  hbr_lcalpha_add_elems(hbr_tagname_set);
  hbr_ucalpha_add_elems(hbr_tagname_set);
  hbr_digit_add_elems(hbr_tagname_set);
  nh_set_int32_add(hbr_tagname_set, ':');
  nh_set_int32_add(hbr_tagname_set, '-');
}

int hbr_tagname_check(hb_char_t c) {
  return nh_set_int32_has(hbr_tagname_set, c);
}
