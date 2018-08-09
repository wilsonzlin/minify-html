#ifndef _HDR_HYPERBUILD_STREAM_HELPER_ATTR
#define _HDR_HYPERBUILD_STREAM_HELPER_ATTR

#include "../../rule/char/attrname.c"
#include "../../rule/char/attrvalquote.c"

#include "../../util/hbchar.h"
#include "../../util/pipe.c"

#include "../streamoptions.c"

#include "./attrval.h"
#include "./quoteattrval.c"
#include "./unquoteattrval.c"

int hbsh_attr(hbs_options_t so, hbu_pipe_t pipe) {
  hbu_buffer_t name = hbu_buffer_create();

  while (1) {
    // Char matched by $attrname required at least once
    hb_char_t c = hbu_pipe_require_predicate(pipe, &hbr_attrname_check, "attribute name");

    if (hbr_ucalpha_check(c)) {
      if (!hbs_options_supressed_error(so, HBE_PARSE_UCASE_ATTR)) {
        hbu_pipe_error(pipe, HBE_PARSE_UCASE_ATTR, "Uppercase letter in attribute name");
      }
      // Lowercase to normalise when checking against rules and closing tag
      hbu_buffer_append(name, c + 32);
    } else {
      hbu_buffer_append(name, c);
    }

    // Don't use hbu_pipe_accept_while_predicate as advanced checks might be needed
    hb_char_t n = hbu_pipe_peek(pipe);

    if (!hbr_attrname_check(n)) {
      break;
    }
  }

  int collapse_and_trim_whitespace = hbu_buffer_compare_lit(name, "class") == 0 &&
                                     so->trim_class_attr;

  hbu_buffer_destroy(name);
  name = NULL;

  if (hbu_pipe_accept_if(pipe, '=')) {
    if (hbr_attrvalquote_check(hbu_pipe_peek(pipe))) {
      // Quoted attribute value
      return hbsh_quoteattrval(so, pipe, collapse_and_trim_whitespace);
    }

    if (!hbs_options_supressed_error(so, HBE_PARSE_UNQUOTED_ATTR)) {
      hbu_pipe_error(pipe, HBE_PARSE_UNQUOTED_ATTR, "Unquoted attribute value");
    }
    // Unquoted attribute value
    hbsh_unquoteattrval(so, pipe);
    return HBSH_ATTR_UNQUOTED;
  }

  return HBSH_ATTR_NOVAL;
}

#endif // _HDR_HYPERBUILD_STREAM_HELPER_ATTR
