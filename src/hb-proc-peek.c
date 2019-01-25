/**
 * Gets the next character.
 * If it's the end, {@link HB_EOD} is returned.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @return character or {@link HB_EOD}
 * @throws on read error
 */
hb_eod_char_t hb_proc_peek_eoi(hbe_err_t *hbe_err, hb_proc_t pipe) {
  // OPTIMISATION: Read directly from buffer if available (no need to unshift later)
  if (pipe->buffer->length) {
    return hb_list_char_get(pipe->buffer, 0);
  }

  hb_eod_char_t c = HBE_CATCH(_hb_proc_read_from_buffer_or_input, pipe);

  if (c != HB_EOD) {
    hb_list_char_unshift(pipe->buffer, c);
  }

  return c;
}

/**
 * Gets the next character.
 * Will cause an error if it's the end and there is no next character.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @return character
 * @throws on read error or HBE_PARSE_UNEXPECTED_END
 */
hb_proc_char_t hb_proc_peek(hbe_err_t *hbe_err, hb_proc_t pipe) {
  hb_eod_char_t c = HBE_CATCH(hb_proc_peek_eoi, pipe);

  HBE_CATCH(_hb_proc_assert_not_eoi, c);

  return c;
}

/**
 * Gets the <i>n</i>th character from current, where <i>n</i> is <code>offset</code>.
 * When <code>offset</code> is 1, the next character is returned (equivalent to {@link hb_proc_peek_eoi_offset}).
 * If <code>offset</code> is after the last character, {@link HB_EOD} is returned.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param offset position of character to get
 * @return character or {@link HB_EOD}
 * @throws on read error
 */
hb_eod_char_t hb_proc_peek_eoi_offset(hbe_err_t *hbe_err, hb_proc_t pipe, size_t offset) {
  HBE_CATCH(_hb_proc_ensure_buffer, pipe, offset);

  hb_eod_char_t c = hb_list_char_get(pipe->buffer, offset - 1);

  return c;
}

/**
 * Gets the <i>n</i>th character from current, where <i>n</i> is <code>offset</code>.
 * When <code>offset</code> is 1, the next character is returned (equivalent to {@link hb_proc_peek_offset}).
 * An error will be caused if <code>offset</code> is after the last character.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param offset position of character to get
 * @return character
 * @throws on read error or HBE_PARSE_UNEXPECTED_END
 */
hb_proc_char_t hb_proc_peek_offset(hbe_err_t *hbe_err, hb_proc_t pipe, size_t offset) {
  hb_eod_char_t c = HBE_CATCH(hb_proc_peek_eoi_offset, pipe, offset);

  HBE_CATCH(_hb_proc_assert_not_eoi, c);

  return c;
}
