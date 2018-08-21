static nh_set_int32_t hbr_whitespace_set;

void hbr_whitespace_add_elems(nh_set_int32_t set) {
  nh_set_int32_add(set, 0x09); // TAB
  nh_set_int32_add(set, 0x0A); // LF
  nh_set_int32_add(set, 0x0C); // FF
  nh_set_int32_add(set, 0x0D); // CR
  nh_set_int32_add(set, 0x20); // SPACE
}

void hbr_whitespace_init(void) {
  hbr_whitespace_set = nh_set_int32_create();
  hbr_whitespace_add_elems(hbr_whitespace_set);
}

int hbr_whitespace_check(hb_char_t c) {
  return nh_set_int32_has(hbr_whitespace_set, c);
}
