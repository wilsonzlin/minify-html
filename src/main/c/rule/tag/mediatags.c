static nh_set_str_t hbr_mediatags_set;

void hbr_mediatags_add_elems(nh_set_str_t set) {
  nh_set_str_add(set, "audio");
  nh_set_str_add(set, "video");
}

void hbr_mediatags_init(void) {
  hbr_mediatags_set = nh_set_str_create();
  hbr_mediatags_add_elems(hbr_mediatags_set);
}

int hbr_mediatags_check(hb_char_t *tag) {
  return nh_set_str_has(hbr_mediatags_set, (char *) tag);
}
