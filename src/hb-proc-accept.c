/**
 * Accepts the next character.
 * Will cause an error if already at end.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @return next character
 * @throws on read/write error or HBE_PARSE_UNEXPECTED_END
 */
hb_proc_char_t hb_proc_accept(hbe_err_t *hbe_err, hb_proc_t pipe) {
  hb_eod_char_t c = HBE_CATCH(_hb_proc_read_from_buffer_or_input, pipe);

  HBE_CATCH(_hb_proc_assert_not_eoi, c);

  _hb_proc_update_pos(pipe, c);

  HBE_CATCH(_hb_proc_write_to_output, pipe, c);

  return c;
}

/**
 * Accepts the next <code>count</code> characters.
 * Requires at least <code>count</code> characters remaining.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param count amount of characters
 * @throws on read/write error or HBE_PARSE_UNEXPECTED_END
 */
void hb_proc_accept_count(hbe_err_t *hbe_err, hb_proc_t pipe, size_t count) {
  for (size_t i = 0; i < count; i++) {
    HBE_CATCH_V(hb_proc_accept, pipe);
  }
}

/**
 * Accepts the following character if it is <code>c</code>.
 * Won't match or cause an error if there are no characters remaining.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param c character to match
 * @return 0 if nothing was accepted, 1 otherwise
 * @throws on read/write error
 */
int hb_proc_accept_if(hbe_err_t *hbe_err, hb_proc_t pipe, hb_proc_char_t c) {
  hb_eod_char_t n = HBE_CATCH(hb_proc_peek_eoi, pipe);

  if (n == HB_EOD || n != c) {
    return 0;
  }

  HBE_CATCH(hb_proc_accept, pipe);

  return 1;
}

/**
 * Accepts the following characters if they match <code>match</code>.
 * Won't match or cause an error if there are not enough characters remaining.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param match characters to match
 * @return 0 if nothing was accepted, 1 otherwise
 * @throws on read/write error
 */
int hb_proc_accept_if_matches(hbe_err_t *hbe_err, hb_proc_t pipe, const char *match) {
  size_t matchedlen = HBE_CATCH(hb_proc_matches, pipe, match);

  int matched = matchedlen > 0;

  if (matched) {
    HBE_CATCH(hb_proc_accept_count, pipe, matchedlen);
  }

  return matched;
}

/**
 * Accepts the following characters if they are either "\r", "\r\n", or "\n".
 * Won't cause an error if insufficient amount of characters left.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @return amount of characters matched
 * @throws on read/write error
 */
size_t hb_proc_accept_if_matches_line_terminator(hbe_err_t *hbe_err, hb_proc_t pipe) {
  size_t matchedlen = HBE_CATCH(hb_proc_matches_line_terminator, pipe);

  if (matchedlen) {
    HBE_CATCH(hb_proc_accept_count, pipe, matchedlen);
  }

  return matchedlen;
}

/**
 * Accepts the following character if it satisfies the predicate <code>pred</code>.
 * Won't do anything if already at the end.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param pred predicate
 * @return 0 if nothing was accepted, 1 otherwise
 * @throws on read/write error
 */
int hb_proc_accept_if_predicate(hbe_err_t *hbe_err, hb_proc_t pipe, hb_proc_predicate_t pred) {
  hb_eod_char_t c = HBE_CATCH(hb_proc_peek_eoi, pipe);

  if (c == HB_EOD || !(*pred)(c)) {
    return 0;
  }

  HBE_CATCH(hb_proc_accept, pipe);

  return 1;
}

/**
 * Accepts every following character until one dissatisfies the predicate <code>pred</code>,
 * or the end is reached.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param pred predicate
 * @return amount of characters accepted
 * @throws on read/write error
 */
size_t hb_proc_accept_while_predicate(hbe_err_t *hbe_err, hb_proc_t pipe, hb_proc_predicate_t pred) {
  size_t count = 0;

  while (1) {
    int matched = HBE_CATCH(hb_proc_accept_if_predicate, pipe, pred);
    if (!matched) {
      break;
    }
    count++;
  }

  return count;
}
