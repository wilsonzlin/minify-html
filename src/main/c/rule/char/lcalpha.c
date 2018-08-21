static nh_set_int32_t hbr_lcalpha_set;

void hbr_lcalpha_add_elems(nh_set_int32_t set) {
  for (char c = 0x61; c <= 0x7A; c++) {
    nh_set_int32_add(set, c);
  }
}

void hbr_lcalpha_init(void) {
  hbr_lcalpha_set = nh_set_int32_create();
  hbr_lcalpha_add_elems(hbr_lcalpha_set);
}

int hbr_lcalpha_check(hb_char_t c) {
  return nh_set_int32_has(hbr_lcalpha_set, c);
}
