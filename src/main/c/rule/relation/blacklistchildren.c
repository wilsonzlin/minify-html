#include "../tag/headingtags.c"
#include "../tag/mediatags.c"
#include "../tag/sectioningtags.c"

static nh_map_str_strset_t hbr_blacklistchildren_map;

void hbr_blacklistchildren_init(void) {
  hbr_blacklistchildren_map = nh_map_str_strset_create();

  // <address>
  nh_set_str_t address = nh_set_str_create();
  hbr_headingtags_add_elems(address);
  hbr_sectioningtags_add_elems(address);
  nh_set_str_add(address, "address");
  nh_set_str_add(address, "header");
  nh_set_str_add(address, "footer");
  nh_map_str_strset_set(hbr_blacklistchildren_map, "address", address);

  // <audio>
  nh_set_str_t audio = nh_set_str_create();
  hbr_mediatags_add_elems(audio);
  nh_map_str_strset_set(hbr_blacklistchildren_map, "audio", audio);

  // <dfn>
  nh_set_str_t dfn = nh_set_str_create();
  nh_set_str_add(dfn, "dfn");
  nh_map_str_strset_set(hbr_blacklistchildren_map, "dfn", dfn);

  // <dt>
  nh_set_str_t dt = nh_set_str_create();
  hbr_headingtags_add_elems(dt);
  hbr_sectioningtags_add_elems(dt);
  nh_set_str_add(dt, "header");
  nh_set_str_add(dt, "footer");
  nh_map_str_strset_set(hbr_blacklistchildren_map, "dt", dt);

  // <footer>
  nh_set_str_t footer = nh_set_str_create();
  nh_set_str_add(footer, "header");
  nh_set_str_add(footer, "footer");
  nh_map_str_strset_set(hbr_blacklistchildren_map, "footer", footer);

  // <form>
  nh_set_str_t form = nh_set_str_create();
  nh_set_str_add(form, "form");
  nh_map_str_strset_set(hbr_blacklistchildren_map, "form", form);

  // <header>
  nh_set_str_t header = nh_set_str_create();
  nh_set_str_add(header, "header");
  nh_set_str_add(header, "footer");
  nh_map_str_strset_set(hbr_blacklistchildren_map, "header", header);

  // <label>
  nh_set_str_t label = nh_set_str_create();
  nh_set_str_add(label, "label");
  nh_map_str_strset_set(hbr_blacklistchildren_map, "label", label);

  // <progress>
  nh_set_str_t progress = nh_set_str_create();
  nh_set_str_add(progress, "progress");
  nh_map_str_strset_set(hbr_blacklistchildren_map, "progress", progress);

  // <th>
  nh_set_str_t th = nh_set_str_create();
  hbr_headingtags_add_elems(th);
  hbr_sectioningtags_add_elems(th);
  nh_set_str_add(th, "header");
  nh_set_str_add(th, "footer");
  nh_map_str_strset_set(hbr_blacklistchildren_map, "th", th);

  // <video>
  nh_set_str_t video = nh_set_str_create();
  hbr_mediatags_add_elems(video);
  nh_map_str_strset_set(hbr_blacklistchildren_map, "video", video);
}

int hbr_blacklistchildren_allowed(hb_char_t *parent, hb_char_t *child) {
  nh_set_str_t set = nh_map_str_strset_get(hbr_blacklistchildren_map, (char *) parent, NULL);
  return set == NULL || !nh_set_str_has(set, (char *) child);
}
