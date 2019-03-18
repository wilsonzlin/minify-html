/**
 * Checks if the next sequence of characters matches the null-terminated character array <code>match</code>
 * of lowercase characters case-insensitively.
 * Won't cause an error if insufficient amount of characters left.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param match characters to match case-insensitively
 * @return amount of characters matched, which should be equal to <code>strlen(match)</code>
 * @throws on read error
 */
size_t hb_proc_matches_i(hbe_err_t *hbe_err, hb_proc_t pipe, const char *match) {
  size_t matchlen = strlen(match);

  for (size_t i = 0; i < matchlen; i++) {
    hb_eod_char_t c = HBE_CATCH(hb_proc_peek_eoi_offset, pipe, i + 1);
    if (!(
      c != HB_EOD &&
      (
        c == match[i] ||
        (hb_rule_ucalpha_check(c) && (c + 32) == match[i])
      )
    )) {
      return 0;
    }
  }

  return matchlen;
}

/**
 * Checks if the next sequence of characters is "\r", "\n", or "\r\n".
 * Won't cause an error if insufficient amount of characters left.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @return amount of characters matched
 * @throws on read error
 */
size_t hb_proc_matches_line_terminator(hbe_err_t *hbe_err, hb_proc_t pipe) {
  // Can't chain into HBE_CATCH(...) || HBE_CATCH(...) || ...
  // as HBE_CATCH needs auxiliary statement

  // `\r\n` must be before `\r`
  size_t crlf = HBE_CATCH(hb_proc_matches, pipe, "\r\n");
  if (crlf) return crlf;

  size_t cr = HBE_CATCH(hb_proc_matches, pipe, "\r");
  if (cr) return cr;

  return HBE_CATCH(hb_proc_matches, pipe, "\n");
}
