#ifndef _HDR_HYPERBUILD_STREAM_HELPER_TAGNAME
#define _HDR_HYPERBUILD_STREAM_HELPER_TAGNAME

#include "../../rule/char/tagname.c"
#include "../../rule/char/ucalpha.c"

#include "../../util/hbchar.h"
#include "../../util/buffer.c"
#include "../../util/pipe.c"

#include "../streamoptions.c"

hbu_buffer_t hbsh_tagname(hbs_options_t so, hbu_pipe_t pipe) {
  hbu_buffer_t name = hbu_buffer_create();

  while (1) {
    hb_char_t c = hbu_pipe_peek(pipe);

    if (!hbr_tagname_check(c)) {
      break;
    }

    if (hbr_ucalpha_check(c)) {
      if (!hbs_options_supressed_error(so, HBE_PARSE_UCASE_TAG)) {
        hbu_pipe_error(pipe, HBE_PARSE_UCASE_TAG, "Uppercase character in tag");
      }
      // Lowercase to normalise when checking against rules and closing tag
      hbu_buffer_append(name, c + 32);
    } else {
      hbu_buffer_append(name, c);
    }

    hbu_pipe_accept(pipe);
  }

  if (!hbs_options_supressed_error(so, HBE_PARSE_NONSTANDARD_TAG) && !hbr_tags_check(hbu_buffer_underlying(name))) {
    hbu_pipe_error(pipe, HBE_PARSE_NONSTANDARD_TAG, "Non-standard tag");
  }

  return name;
}

#endif // _HDR_HYPERBUILD_STREAM_HELPER_TAGNAME
