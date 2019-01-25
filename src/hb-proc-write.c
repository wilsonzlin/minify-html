/**
 * Writes a character.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param c character
 * @throws on write error
 */
void hb_proc_write(hbe_err_t *hbe_err, hb_proc_t pipe, hb_proc_char_t c) {
  HBE_CATCH_V(_hb_proc_write_to_output, pipe, c);
}

/**
 * Writes a buffer.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param buffer buffer
 * @throws on write error
 */
void hb_proc_write_buffer(hbe_err_t *hbe_err, hb_proc_t pipe, hb_list_char_t buffer) {
  for (size_t i = 0; i < buffer->length; i++) {
    HBE_CATCH_V(_hb_proc_write_to_output, pipe, hb_list_char_get(buffer, i));
  }
}

/**
 * Writes a character from its Unicode code point in UTF-8 encoding, which may be multiple bytes.
 *
 * @param hbe_err pointer to hbe_err_t
 * @param pipe pipe
 * @param code_point Unicode code point
 * @return amount of bytes written (0 if invalid code point)
 * @throws on write error
 */
// Logic copied from https://gist.github.com/MightyPork/52eda3e5677b4b03524e40c9f0ab1da5
size_t hb_proc_write_unicode(hbe_err_t *hbe_err, hb_proc_t pipe, uint32_t code_point) {
   if (code_point <= 0x7F) {
    // Plain ASCII
    HBE_CATCH(hb_proc_write, pipe, (hb_proc_char_t) code_point);
    return 1;
  }

  if (code_point <= 0x07FF) {
    // 2-byte unicode
    HBE_CATCH(hb_proc_write, pipe, (hb_proc_char_t) (((code_point >> 6) & 0x1F) | 0xC0));
    HBE_CATCH(hb_proc_write, pipe, (hb_proc_char_t) (((code_point >> 0) & 0x3F) | 0x80));
    return 2;
  }

  if (code_point <= 0xFFFF) {
    // 3-byte unicode
    HBE_CATCH(hb_proc_write, pipe, (hb_proc_char_t) (((code_point >> 12) & 0x0F) | 0xE0));
    HBE_CATCH(hb_proc_write, pipe, (hb_proc_char_t) (((code_point >>  6) & 0x3F) | 0x80));
    HBE_CATCH(hb_proc_write, pipe, (hb_proc_char_t) (((code_point >>  0) & 0x3F) | 0x80));
    return 3;
  }

  if (code_point <= 0x10FFFF) {
    // 4-byte unicode
    HBE_CATCH(hb_proc_write, pipe, (hb_proc_char_t) (((code_point >> 18) & 0x07) | 0xF0));
    HBE_CATCH(hb_proc_write, pipe, (hb_proc_char_t) (((code_point >> 12) & 0x3F) | 0x80));
    HBE_CATCH(hb_proc_write, pipe, (hb_proc_char_t) (((code_point >>  6) & 0x3F) | 0x80));
    HBE_CATCH(hb_proc_write, pipe, (hb_proc_char_t) (((code_point >>  0) & 0x3F) | 0x80));
    return 4;
  }

  return 0;
}
