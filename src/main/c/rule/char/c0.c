static nh_set_int32_t hbr_c0_set;

void hbr_c0_add_elems(nh_set_int32_t set) {
  for (char c = 0x0; c <= 0x1F; c++) {
    nh_set_int32_add(set, c);
  }
}

void hbr_c0_init(void) {
  hbr_c0_set = nh_set_int32_create();
  hbr_c0_add_elems(hbr_c0_set);
}

int hbr_c0_check(hb_char_t c) {
  return nh_set_int32_has(hbr_c0_set, c);
}
