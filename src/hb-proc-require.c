/**
 * Requires the next character to be <code>c</code>.
 * The matched character is written to output.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param c character to match
 * @throws on read/write error, HBE_PARSE_UNEXPECTED_END, or HBE_PARSE_EXPECTED_NOT_FOUND
 */
void hb_proc_require(hbe_err_t *hbe_err, hb_proc_t pipe, hb_proc_char_t c) {
  hb_proc_char_t n = HBE_CATCH_V(hb_proc_accept, pipe);

  if (c != n) {
    hb_proc_THROW_V(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected `%c` (0x%x), got `%c` (0x%x)", c, c, n, n);
  }
}

/**
 * Requires the next character to be <code>c</code>.
 * The matched character is skipped over and NOT written to output, and also returned.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param c character to match
 * @return matched character
 * @throws on read/write error, HBE_PARSE_UNEXPECTED_END, or HBE_PARSE_EXPECTED_NOT_FOUND
 */
hb_proc_char_t hb_proc_require_skip(hbe_err_t *hbe_err, hb_proc_t pipe, hb_proc_char_t c) {
  hb_proc_char_t n = HBE_CATCH(hb_proc_skip, pipe);

  if (c != n) {
    hb_proc_THROW(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected `%c` (0x%x), got `%c` (0x%x) at %s", c, c, n, n);
  }

  return n;
}

/**
 * Requires the next character to satisfy the predicate <code>pred</code>.
 * The matched character is written to output.
 * If not matched, the error message will describe the expected output using <code>name</code>.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param pred predicate
 * @param name what to output in the error message to describe the requirement
 * @return required character
 * @throws on read/write error, HBE_PARSE_UNEXPECTED_END, or HBE_PARSE_EXPECTED_NOT_FOUND
 */
hb_proc_char_t hb_proc_require_predicate(hbe_err_t *hbe_err, hb_proc_t pipe, hb_proc_predicate_t pred, const char *name) {
  hb_proc_char_t n = HBE_CATCH(hb_proc_accept, pipe);

  if (!(*pred)(n)) {
    hb_proc_THROW(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected %s, got `%c` (0x%x)", name, n, n);
  }

  return n;
}

/**
 * Requires the next character to satisfy the predicate <code>pred</code>.
 * The matched character is skipped over and NOT written to output.
 * If not matched, the error message will describe the expected output using <code>name</code>.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param pred predicate
 * @param name what to output in the error message to describe the requirement
 * @return required character
 * @throws on read/write error, HBE_PARSE_UNEXPECTED_END, or HBE_PARSE_EXPECTED_NOT_FOUND
 */
hb_proc_char_t hb_proc_require_skip_predicate(hbe_err_t *hbe_err, hb_proc_t pipe, hb_proc_predicate_t pred, const char *name) {
  hb_proc_char_t n = HBE_CATCH(hb_proc_skip, pipe);

  if (!(*pred)(n)) {
    hb_proc_THROW(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected %s, got `%c` (0x%x)", name, n, n);
  }

  return n;
}

/**
 * Requires the next sequence of characters to be equal to <code>match</code>.
 * Matched characters are written to output.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param match sequence of characters to require
 * @throws on read/write error, HBE_PARSE_UNEXPECTED_END, or HBE_PARSE_EXPECTED_NOT_FOUND
 */
void hb_proc_require_match(hbe_err_t *hbe_err, hb_proc_t pipe, const char *match) {
  int matches = HBE_CATCH_V(hb_proc_accept_if_matches, pipe, match);
  if (!matches) {
    hb_proc_THROW_V(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected `%s`", match);
  }
}

/**
 * Requires the next sequence of characters to be equal to <code>match</code>.
 * Matched characters are skipped over and NOT written to output.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param match sequence of characters to require
 * @throws on read/write error, HBE_PARSE_UNEXPECTED_END, or HBE_PARSE_EXPECTED_NOT_FOUND
 */
void hb_proc_require_skip_match(hbe_err_t *hbe_err, hb_proc_t pipe, const char *match) {
  int matches = HBE_CATCH_V(hb_proc_skip_if_matches, pipe, match);
  if (!matches) {
    hb_proc_THROW_V(pipe, HBE_PARSE_EXPECTED_NOT_FOUND, "Expected `%s`", match);
  }
}
