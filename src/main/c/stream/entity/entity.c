#include <inttypes.h>

// NOTE: Do not use 0
#define HBS_ENTITY_TYPE_NAME 1
#define HBS_ENTITY_TYPE_DECIMAL 2
#define HBS_ENTITY_TYPE_HEXADECIMAL 3

#define HBS_ENTITY_MAX_ENTITY_LENGTH 10 // Don't try to parse malformed entities forever

static void _hbs_entity_interr_unknown_entity(hbe_err_t *hbe_err, int type) {
  HBE_THROW_V(HBE_INTERR_UNKNOWN_ENTITY_TYPE, "INTERR %d is not a known entity type", type);
}

static void _hbs_entity_write_literal(hbe_err_t *hbe_err, hbu_pipe_t pipe, int type, hbu_list_char_t entity_raw, int consumed_semicolon) {
  HBE_CATCH_V(hbu_pipe_write, pipe, '&');
  if (type == HBS_ENTITY_TYPE_HEXADECIMAL || type == HBS_ENTITY_TYPE_DECIMAL) {
    HBE_CATCH_V(hbu_pipe_write, pipe, '#');
    if (type == HBS_ENTITY_TYPE_HEXADECIMAL) {
      HBE_CATCH_V(hbu_pipe_write, pipe, 'x');
    }
  }

  if (entity_raw != NULL) {
    HBE_CATCH_V(hbu_pipe_write_buffer, pipe, entity_raw);
  }

  if (consumed_semicolon) {
    HBE_CATCH_V(hbu_pipe_write, pipe, ';');
  }
}

static void _hbs_entity_syntax_error(hbe_err_t *hbe_err, hbu_streamoptions_t so, hbu_pipe_t pipe, int type, hbu_list_char_t entity_raw, int consumed_semicolon, hbe_errcode_t errcode, const char *reason) {
  if (hbu_streamoptions_supressed_error(so, errcode)) {
    HBE_CATCH_V(_hbs_entity_write_literal, pipe, type, entity_raw, consumed_semicolon);
    return;
  }

  HBU_PIPE_THROW_V(pipe, errcode, reason);
}

// NOTE: Return 0 if syntax error but suppressed
static int _hbs_entity_process_prefix(hbe_err_t *hbe_err, hbu_pipe_t pipe, hbu_streamoptions_t so) {
  hb_char_t c = HBE_CATCH(hbu_pipe_peek, pipe);

  if (hbr_lcalpha_check(c) || hbr_ucalpha_check(c)) {
    // Name-based entity
    return HBS_ENTITY_TYPE_NAME;
  }

  hb_eod_char_t c2 = HBE_CATCH(hbu_pipe_peek_eoi_offset, pipe, 2);

  if (c == '#' && c2 == 'x') {
    // Hexadecimal-based entity
    // NOTE: Check before decimal-based
    HBE_CATCH(hbu_pipe_skip_amount, pipe, 2);
    return HBS_ENTITY_TYPE_HEXADECIMAL;
  }

  if (c == '#') {
    // Decimal-based entity
    HBE_CATCH(hbu_pipe_skip, pipe);
    return HBS_ENTITY_TYPE_DECIMAL;
  }

  // Malformed entity
  HBE_PASS(_hbs_entity_syntax_error, so, pipe, -1, NULL, 0, HBE_PARSE_MALFORMED_ENTITY, "Invalid character after ampersand");
}

void hbs_entity(hbe_err_t *hbe_err, hbu_streamoptions_t so, hbu_pipe_t pipe) {
  hbu_list_char_t entity_raw = NULL;

  HBE_CATCH_F(hbu_pipe_require_skip, pipe, '&');

  // Quickly check and short circuit if BARE_AMPERSAND is suppressed
  // and next character is whitespace
  if (hbu_streamoptions_supressed_error(so, HBE_PARSE_BARE_AMPERSAND)) {
    hb_eod_char_t next = HBE_CATCH_F(hbu_pipe_peek_eoi, pipe);
    if (hbr_whitespace_check(next)) {
      HBE_CATCH_F(hbu_pipe_write, pipe, '&');
      return;
    }
  }

  int type = HBE_CATCH_F(_hbs_entity_process_prefix, pipe, so);
  if (!type) {
    // Syntax error that was suppressed
    goto finally;
  }

  entity_raw = hbu_list_char_create_size(0, HBS_ENTITY_MAX_ENTITY_LENGTH + 1);
  int under_max = 0;

  for (int i = 0; i < HBS_ENTITY_MAX_ENTITY_LENGTH; i++) {
    hb_char_t e = HBE_CATCH_F(hbu_pipe_skip, pipe);

    if (e == ';') {
      under_max = 1;
      break;
    }

    int well_formed = 0;

    switch (type) {
    case HBS_ENTITY_TYPE_NAME:
      well_formed = hbr_lcalpha_check(e) || hbr_ucalpha_check(e);
      break;

    case HBS_ENTITY_TYPE_DECIMAL:
      well_formed = hbr_digit_check(e);
      break;

    case HBS_ENTITY_TYPE_HEXADECIMAL:
      well_formed = hbr_hex_check(e);
      break;

    default:
      HBE_PASS_F(_hbs_entity_interr_unknown_entity, type);
    }

    if (!well_formed) {
      HBE_PASS_F(_hbs_entity_syntax_error, so, pipe, type, entity_raw, 0, HBE_PARSE_MALFORMED_ENTITY, "Characters after ampersand don't form entity");
    }

    hbu_list_char_append(entity_raw, e);
  }

  if (!under_max) {
    // Malformed entity (too long)
    HBE_PASS_F(_hbs_entity_syntax_error, so, pipe, type, entity_raw, 0, HBE_PARSE_MALFORMED_ENTITY, "Malformed entity");
  }

  int valid = 1;
  hb_char_t *entity_raw_u = hbu_list_char_underlying(entity_raw);
  uintmax_t code_point;

  switch (type) {
  case HBS_ENTITY_TYPE_NAME:
    valid = hbr_entityrefs_check(entity_raw_u);
    if (valid && so->decode_entities) {
      HBE_CATCH_F(hbu_pipe_write, pipe, hbr_entityrefs_get(entity_raw_u));
    }
    break;

  case HBS_ENTITY_TYPE_DECIMAL:
  case HBS_ENTITY_TYPE_HEXADECIMAL:
    code_point = strtoumax((char *) entity_raw_u, NULL, (type == HBS_ENTITY_TYPE_DECIMAL) ? 10 : 16);
    valid = errno == 0 && code_point <= 0x10FFFF;
    if (valid && so->decode_entities) {
      valid = HBE_CATCH_F(hbu_pipe_write_unicode, pipe, code_point);
    }
    break;

  default:
    HBE_PASS_F(_hbs_entity_interr_unknown_entity, type);
  }

  if (!valid) {
    HBE_PASS_F(_hbs_entity_syntax_error, so, pipe, type, entity_raw, 1, HBE_PARSE_INVALID_ENTITY, "Invalid entity");
  }

  if (!so->decode_entities) {
    HBE_CATCH_F(_hbs_entity_write_literal, pipe, type, entity_raw, 1);
  }

  finally:
    if (entity_raw != NULL) {
      hbu_list_char_destroy(entity_raw);
      entity_raw = NULL;
    }
}
