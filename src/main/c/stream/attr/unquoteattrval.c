#include "../entity/entity.c"

void hbs_unquoteattrval(hbe_err_t *hbe_err, hbu_streamoptions_t so, hbu_pipe_t pipe) {
  // Can't use hbu_pipe_require_predicate, as first char might be `&`,
  // which needs to be processed by hbs_entity
  int at_least_one_char = 0;

  hb_char_t c;
  while (1) {
    c = HBE_CATCH_V(hbu_pipe_peek, pipe);

    if (!hbr_unquotedattrval_check(c)) {
      return;
    }

    at_least_one_char = 1;

    if (c == '&') {
      HBE_CATCH_V(hbs_entity, so, pipe);
    } else {
      HBE_CATCH_V(hbu_pipe_accept, pipe);
    }
  }

  if (!at_least_one_char) {
    HBU_PIPE_THROW_V(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected unquoted attribute value, got `%c` (0x%x)", c);
  }
}
