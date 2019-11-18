#include <getopt.h>
#include <hbcli/cli.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <hb/hyperbuild.h>
#include <hb/err.h>


nh_set_str_t hbu_streamoptions_parse_list_of_tags(hbe_err_t *hbe_err, char *argv) {
  nh_set_str_t set = NULL;
  hb_list_charlist_t list = NULL;

  if (argv != NULL && strcmp(argv, "*")) {
    return NULL;
  }

  set = nh_set_str_create();

  if (argv == NULL) {
    return set;
  }

  list = hb_list_charlist_create_from_split((hb_proc_char_t *) argv, ',');

  for (size_t i = 0; i < list->length; i++) {
    hb_list_char_t part = hb_list_charlist_get(list, i);
    hb_proc_char_t *part_c = hb_list_char_underlying(part);

    if (hb_list_char_get(part, 0) == '$') {
      // Set of tags
      hb_list_char_shift(part);
      HBE_CATCH_F(hbu_streamoptions_parse_and_add_tag_set, (char *) part_c, set);

    } else {
      // Single tag
      if (!hb_rule_tags_check(part_c)) {
        HBE_THROW_F(HBE_CLI_INVALID_TAG, "%s is not a standard tag and was provided as part of an argument's value", part_c);
      }
      nh_set_str_add(set, (char *) hb_list_char_underlying_copy(part));
    }
  }

  finally:
    if (list != NULL) {
      hb_list_charlist_destroy_from_split(list);
      list = NULL;
    }
    if (*hbe_err != NULL) {
      if (set != NULL) {
        nh_set_str_destroy(set);
        set = NULL;
      }
    }
    return set;
}





void hbu_streamoptions_parse_and_add_tag_set(hbe_err_t *hbe_err, char *set_name, nh_set_str_t set) {
  if (strcmp(set_name, "content") == 0) {
    hb_rule_contenttags_add_elems(set);
  } else if (strcmp(set_name, "contentfirst") == 0) {
    hb_rule_contentfirsttags_add_elems(set);
  } else if (strcmp(set_name, "formatting") == 0) {
    hb_rule_formattingtags_add_elems(set);
  } else if (strcmp(set_name, "layout") == 0) {
    hb_rule_layouttags_add_elems(set);
  } else if (strcmp(set_name, "specific") == 0) {
    hb_rule_specifictags_add_elems(set);
  } else if (strcmp(set_name, "heading") == 0) {
    hb_rule_headingtags_add_elems(set);
  } else if (strcmp(set_name, "media") == 0) {
    hb_rule_mediatags_add_elems(set);
  } else if (strcmp(set_name, "sectioning") == 0) {
    hb_rule_sectioningtags_add_elems(set);
  } else if (strcmp(set_name, "void") == 0) {
    hb_rule_voidtags_add_elems(set);
  } else if (strcmp(set_name, "wss") == 0) {
    hb_rule_wsstags_add_elems(set);
  } else {
    HBE_THROW_V(HBE_CLI_INVALID_TAG_SET, "Unrecognised tag set `%s`", set_name);
  }
}

int main(int argc, char **argv) {
  hyperbuild_init();




  hb_rune* output = hyperbuild_from_file(input_path, &cfg, &result);
  if (result->code != HB_ERR_OK) {
    // An error occurred.
    _cli_error(result->msg);
  }

  open(output_path, )
}
