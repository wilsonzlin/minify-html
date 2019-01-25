/**
 * Skips over the next character.
 * Requires that the file has at least one character remaining.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @return skipped character
 * @throws on read error or HBE_PARSE_UNEXPECTED_END
 */
hb_proc_char_t hb_proc_skip(hbe_err_t *hbe_err, hb_proc_t pipe) {
  hb_eod_char_t c = HBE_CATCH(_hb_proc_read_from_buffer_or_input, pipe);

  HBE_CATCH(_hb_proc_assert_not_eoi, c);

  _hb_proc_update_pos(pipe, c);

  return c;
}

/**
 * Skips over the next <code>amount</code> characters.
 * Requires that the file has at least <code>amount</code> characters remaining.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param amount amount of characters to skip
 * @throws on read error or HBE_PARSE_UNEXPECTED_END
 */
void hb_proc_skip_amount(hbe_err_t *hbe_err, hb_proc_t pipe, size_t amount) {
  for (size_t i = 0; i < amount; i++) {
    HBE_CATCH_V(hb_proc_skip, pipe);
  }
}

/**
 * Skips over the following character if it is <code>c</code>.
 * Won't cause an error if the end is reached.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param c character to skip if next
 * @return 1 if skipped, 0 otherwise
 * @throws on read error
 */
int hb_proc_skip_if(hbe_err_t *hbe_err, hb_proc_t pipe, hb_proc_char_t c) {
  hb_eod_char_t n = HBE_CATCH(hb_proc_peek_eoi, pipe);

  if (n == HB_EOD || n != c) {
    return 0;
  }

  HBE_CATCH(hb_proc_skip, pipe);
  return 1;
}

/**
 * Skips over every following character until one dissatisfies the predicate <code>pred</code>,
 * or the end is reached.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param pred predicate
 * @return amount of characters skipped
 * @throws on read error
 */
size_t hb_proc_skip_while_predicate(hbe_err_t *hbe_err, hb_proc_t pipe, hb_proc_predicate_t pred) {
  size_t count = 0;

  while (1) {
    hb_eod_char_t c = HBE_CATCH(hb_proc_peek_eoi, pipe);

    if (c == HB_EOD || !(*pred)(c)) {
      break;
    }

    HBE_CATCH(hb_proc_skip, pipe);
    count++;
  }

  return count;
}

/**
 * Skips over the next sequence of characters if they are <code>match</code>.
 * Requires that the file has at least the length of <code>match</code> characters remaining.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param match sequence of characters to match
 * @return 0 if not matched, 1 otherwise
 * @throws on read error or HBE_PARSE_UNEXPECTED_END
 */
int hb_proc_skip_if_matches(hbe_err_t *hbe_err, hb_proc_t pipe, const char *match) {
  size_t matchedlen = HBE_CATCH(hb_proc_matches, pipe, match);

  int matched = matchedlen > 0;

  if (matched) {
    HBE_CATCH(hb_proc_skip_amount, pipe, matchedlen);
  }

  return matched;
}
