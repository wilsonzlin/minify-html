#include "../entity/entity.c"

void hbs_unquoteattrval(hbe_err_t *hbe_err, hbu_streamoptions_t so, hb_proc_t pipe) {
  // Can't use hb_proc_require_predicate, as first char might be `&`,
  // which needs to be processed by hbs_entity
  int at_least_one_char = 0;

  hb_proc_char_t c;
  while (1) {
    c = HBE_CATCH_V(hb_proc_peek, pipe);

    if (!hb_rule_unquotedattrval_check(c)) {
      return;
    }

    at_least_one_char = 1;

    if (c == '&') {
      HBE_CATCH_V(hbs_entity, so, pipe);
    } else {
      HBE_CATCH_V(hb_proc_accept, pipe);
    }
  }

  if (!at_least_one_char) {
    hb_proc_THROW_V(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected unquoted attribute value, got `%c` (0x%x)", c);
  }
}
