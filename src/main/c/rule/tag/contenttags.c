static nh_set_str_t hbr_contenttags_set;

void hbr_contenttags_add_elems(nh_set_str_t set) {
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

void hbr_contenttags_init(void) {
  hbr_contenttags_set = nh_set_str_create();
  hbr_contenttags_add_elems(hbr_contenttags_set);
}

int hbr_contenttags_check(hb_char_t *tag) {
  return nh_set_str_has(hbr_contenttags_set, (char *) tag);
}
