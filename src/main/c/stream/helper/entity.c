#ifndef _HDR_HYPERBUILD_STREAM_HELPER_ENTITY
#define _HDR_HYPERBUILD_STREAM_HELPER_ENTITY

#include <inttypes.h>

#include "../util/hbchar.h"
#include "../util/buffer.c"
#include "../util/pipe.c"

#include "../rule/entity/entityrefs.c"
#include "../rule/char/lcalpha.c"
#include "../rule/char/ucalpha.c"
#include "../rule/char/digit.c"
#include "../rule/char/hex.c"

#include "./streamoptions.c"

#define HBSH_ENTITY_TYPE_NAME 1
#define HBSH_ENTITY_TYPE_DECIMAL 2
#define HBSH_ENTITY_TYPE_HEXADECIMAL 3

#define HBSH_ENTITY_MAX_ENTITY_LENGTH 10 // Don't try to parse malformed entities forever

static void _hbsh_entity_interr_unknown_entity(void) {
  hbe_fatal(HBE_INTERR_UNKNOWN_ENTITY_TYPE, "INTERR $type is not a known entity type");
}

static void _hbsh_entity_write_literal(hbu_pipe_t pipe, int type, hbu_buffer_t entity_raw, int consumed_semicolon) {
  hbu_pipe_write(pipe, '&');
  if (type == HBSH_ENTITY_TYPE_HEXADECIMAL || type == HBSH_ENTITY_TYPE_DECIMAL) {
    hbu_pipe_write(pipe, '#');
    if (type == HBSH_ENTITY_TYPE_HEXADECIMAL) {
      hbu_pipe_write(pipe, 'x');
    }
  }

  hbu_pipe_write_buffer(pipe, entity_raw);

  if (consumed_semicolon) {
    hbu_pipe_write(pipe, ';');
  }
}

static void _hbsh_entity_handle_error(hbs_options_t so, hbu_pipe_t pipe, int type, hbu_buffer_t entity_raw, int consumed_semicolon, hbe_errcode_t errcode, const char *reason) {
  if (hbs_options_supressed_error(so, errcode)) {
    _hbsh_entity_write_literal(pipe, type, entity_raw, consumed_semicolon);
    hbu_buffer_destroy(entity_raw);
    return;
  }

  hbu_pipe_error(pipe, errcode, reason);
}

void hbsh_entity(hbs_options_t so, hbu_pipe_t pipe) {
  hbu_pipe_require_skip(pipe, '&');

  // Quickly check and short circuit if BARE_AMPERSAND is suppressed
  // and next character is whitespace
  if (hbs_options_supressed_error(so, HBE_PARSE_BARE_AMPERSAND) &&
      hbr_whitespace_check(hbu_pipe_peek_eoi(pipe))) {
    hbu_pipe_write(pipe, '&');
    return;
  }

  hb_char_t c = hbu_pipe_peek(pipe);

  // _hbsh_entity_handle_error will free this in case of error
  hbu_buffer_t entity_raw = hbu_buffer_create_size(HBSH_ENTITY_MAX_ENTITY_LENGTH + 1);

  int type = -1;

  if (hbr_lcalpha_check(c) || hbr_ucalpha_check(c)) {
    // Name-based entity
    type = HBSH_ENTITY_TYPE_NAME;

  } else if (c == '#' && hbu_pipe_peek_offset(pipe, 2) == 'x') {
    // Hexadecimal-based entity
    // NOTE: Check before decimal-based
    hbu_pipe_skip_amount(pipe, 2);
    type = HBSH_ENTITY_TYPE_HEXADECIMAL;

  } else if (c == '#') {
    // Decimal-based entity
    hbu_pipe_skip(pipe);
    type = HBSH_ENTITY_TYPE_DECIMAL;

  } else {
    // Malformed entity
    return _hbsh_entity_handle_error(so, pipe, type, entity_raw, 0, HBE_PARSE_MALFORMED_ENTITY, "Invalid character after ampersand");
  }

  int under_max = 0;

  for (int i = 0; i < HBSH_ENTITY_MAX_ENTITY_LENGTH; i++) {
    hb_char_t e = hbu_pipe_skip(pipe);

    if (e == ';') {
      under_max = 1;
      break;
    }

    int well_formed = 0;

    switch (type) {
    case HBSH_ENTITY_TYPE_NAME:
      well_formed = hbr_lcalpha_check(e) || hbr_ucalpha_check(e);
      break;

    case HBSH_ENTITY_TYPE_DECIMAL:
      well_formed = hbr_digit_check(e);
      break;

    case HBSH_ENTITY_TYPE_HEXADECIMAL:
      well_formed = hbr_hex_check(e);
      break;

    default:
      _hbsh_entity_interr_unknown_entity();
    }

    if (!well_formed) {
      return _hbsh_entity_handle_error(so, pipe, type, entity_raw, 0, HBE_PARSE_MALFORMED_ENTITY, "Characters after ampersand don't form entity");
    }

    hbu_buffer_append(entity_raw, e);
  }

  if (!under_max) {
    // Malformed entity (too long)
    return _hbsh_entity_handle_error(so, pipe, type, entity_raw, 0, HBE_PARSE_MALFORMED_ENTITY, "Malformed entity");
  }

  int valid = 1;
  hb_char_t *entity_raw_u = hbu_buffer_underlying(entity_raw);
  uintmax_t code_point;

  switch (type) {
  case HBSH_ENTITY_TYPE_NAME:
    valid = hbr_entityrefs_check(entity_raw_u);
    if (valid && so->decode_entities) {
      hbu_pipe_write(pipe, hbr_entityrefs_get(entity_raw_u));
    }
    break;

  case HBSH_ENTITY_TYPE_DECIMAL:
  case HBSH_ENTITY_TYPE_HEXADECIMAL:
    code_point = strtoumax((char *) entity_raw_u, NULL, (type == HBSH_ENTITY_TYPE_DECIMAL) ? 10 : 16);
    valid = errno == 0 && code_point <= 0x10FFFF;
    if (valid && so->decode_entities) {
      valid = hbu_pipe_write_unicode(pipe, code_point);
    }
    break;

  default:
    _hbsh_entity_interr_unknown_entity();
  }

  if (!valid) {
    return _hbsh_entity_handle_error(so, pipe, type, entity_raw, 1, HBE_PARSE_INVALID_ENTITY, "Invalid entity");
  }

  if (!so->decode_entities) {
    _hbsh_entity_write_literal(pipe, type, entity_raw, 1);
  }
}

#endif // _HDR_HYPERBUILD_STREAM_HELPER_ENTITY
