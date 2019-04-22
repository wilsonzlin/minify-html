#include <hb/unit.h>
#include <hb/proc.h>
#include <hb/rule.h>

// The minimum length of any entity is 3, which is a character entity reference with a single character name.
// The longest UTF-8 representation of a Unicode code point is 4 bytes.
// Because there are no character entity references with a name of length 1, it's always better to decode entities for minification purposes.

// Based on the data sourced from https://www.w3.org/TR/html5/entities.json as of 2019-04-20T04:00:00.000Z:
// - Entity names can have [A-Za-z0-9] characters, and are case sensitive.
// - Some character entity references do not need to end with a semicolon.
// - The longest name is "CounterClockwiseContourIntegral", with length 31 (excluding leading ampersand and trailing semicolon).
// - All entity names are at least 2 characters long.

// Browser implementation behaviour to consider:
// - It is unclear what happens if an entity name does not match case sensitively but matches two or more case insensitively.
//   - For example, given "AlphA" or "aLpha", does the browser choose "alpha" or "Alpha"?
// - Do browsers render valid entities without trailing semicolons?
//   - For example, how do browsers interpret "Chuck-&amp-Cheese", "1&amp1", and "&ampe;"?

// hyperbuild implementation:
// - Entities must start with an ampersand and end with a semicolon.
// - Once an ampersand is encountered, it and the sequence of characters following must match the following ECMAScript regular expression to be considered a well formed entity:
//
//   /&(#(x[0-9a-f]{1-6}|[0-9]{1,7}))|[a-z0-9]{2,31};/i
//
// - If the sequence of characters following an ampersand do not combine to form a well formed entity, the ampersand is considered a bare ampersand.
//   - A bare ampersand is an ampersand that is interpreted literally and not as the start of an entity.
//   - hyperbuild looks ahead without consuming to check if the following characters would form a well formed entity. If they don't, only the longest subsequence that could form a well formed entity is consumed.
// - An entity is considered invalid if it is well formed but represents a non-existent Unicode code point or reference name.

#define _MAX_UNICODE_CODE_POINT 0x10FFFF

typedef enum {
    _TYPE_MALFORMED,
    _TYPE_NAME,
    _TYPE_DECIMAL,
    _TYPE_HEXADECIMAL
} _type;

typedef bool _valid_char_predicate(hb_rune c);

static int32_t _parse_decimal(nh_view_str* view) {
    int32_t val = 0;
    nh_view_for(view, i, _, len) {
        char c = nh_view_str_get(view, i);
        val = val * 10 + (c - '0');
    }
    return val > _MAX_UNICODE_CODE_POINT ? -1 : val;
}

static int32_t _parse_hexadecimal(nh_view_str* view) {
    int32_t val = 0;
    nh_view_for(view, i, _, len) {
        char c = nh_view_str_get(view, i);
        int32_t digit =
                hb_rule_ascii_digit_check(c) ? c - '0' :
                hb_rule_ascii_uppercase_check(c) ? c - 'A' + 10 :
                c - 'a' + 10;
        val = val * 16 + digit;
    }
    return val > _MAX_UNICODE_CODE_POINT ? -1 : val;
}

/**
 * Process an HTML entity.
 *
 * @return Unicode code point of the entity, or HB_UNIT_ENTITY_NONE if the entity is malformed or invalid
 */
int32_t hb_unit_entity(hb_proc* proc) {
    // View of the entire entity, including leading ampersand and any trailing semicolon.
    hb_proc_view_init_src(entity, proc);
    hb_proc_view_start_with_src_next(&entity, proc);
    hb_proc_require_skip(proc, '&');

    // The input can end at any time after initial ampersand.
    // Examples of valid complete source code: "&", "&a", "&#", "&#09", "&amp".

    // There are three stages to this function:
    //
    // 1. Determine the type of entity, so we can know how to parse and validate the following characters.
    //    - This can be done by simply looking at the first and second characters after the initial ampersand, e.g. "&#", "&#x", "&a".
    // 2. Parse the entity data, i.e. the characters between the ampersand and semicolon.
    //    - To avoid parsing forever on malformed entities without semicolons, there is an upper bound on the amount of possible characters, based on the type of entity detected from the first stage.
    // 3. Interpret and validate the data.
    //    - This simply checks if it refers to a valid Unicode code point or entity reference name.

    // First stage: determine the type of entity.
    _valid_char_predicate* predicate;
    _type type;
    size_t min_len;
    size_t max_len;

    if (hb_proc_skip_if_matches(proc, "#x")) {
        predicate = &hb_rule_ascii_hex_check;
        type = _TYPE_HEXADECIMAL;
        min_len = 1;
        max_len = 6;

    } else if (hb_proc_skip_if(proc, '#')) {
        predicate = &hb_rule_ascii_digit_check;
        type = _TYPE_DECIMAL;
        min_len = 1;
        max_len = 7;

    } else if (hb_rule_entity_reference_valid_name_char(hb_proc_peek_eof(proc))) {
        predicate = &hb_rule_entity_reference_valid_name_char;
        type = _TYPE_NAME;
        min_len = 2;
        max_len = 31;

    } else {
        hb_proc_error_if_not_suppressed(proc, HB_ERR_PARSE_MALFORMED_ENTITY, "Malformed entity");
        // Output bare ampersand.
        hb_proc_write(proc, '&');
        return HB_UNIT_ENTITY_NONE;
    }

    // Second stage: try to parse a well formed entity.
    // If the entity is not well formed, either throw an error or interpret literally (depending on configuration).
    hb_proc_view_init_src(data, proc);
    hb_proc_view_start_with_src_next(&data, proc);
    for (size_t i = 0; i < max_len; i++) {
        hb_eof_rune c = hb_proc_peek_eof(proc);
        // Character ends entity.
        if (c == ';') {
            break;
        }
        // Character would not form well formed entity.
        if (!(*predicate)(c)) {
            type = _TYPE_MALFORMED;
            break;
        }
        // Character is valid.
        hb_proc_skip(proc);
    }
    hb_proc_view_end_with_src_prev(&data, proc);
    if (nh_view_str_length(&data) < min_len) type = _TYPE_MALFORMED;
    // Don't try to consume semicolon if entity is not well formed already.
    if (type != _TYPE_MALFORMED && !hb_proc_skip_if(proc, ';')) type = _TYPE_MALFORMED;
    hb_proc_view_end_with_src_prev(&entity, proc);

    if (type == _TYPE_MALFORMED) {
        hb_proc_error_if_not_suppressed(proc, HB_ERR_PARSE_MALFORMED_ENTITY, "Malformed entity");
        // Write longest subsequence of characters that could form a well formed entity.
        hb_proc_write_view(proc, &entity);
        return HB_UNIT_ENTITY_NONE;
    }

    // Third stage: validate entity and decode if configured to do so.
    int32_t uchar = -1;
    switch (type) {
        case _TYPE_NAME:
            uchar = hb_rule_entity_reference_get_code_point(&data);
            break;

        case _TYPE_DECIMAL:
            uchar = _parse_decimal(&data);
            break;

        case _TYPE_HEXADECIMAL:
            uchar = _parse_hexadecimal(&data);
            break;

        default:
            // Defensive coding.
            hb_proc_error(proc, HB_ERR_INTERR_UNKNOWN_ENTITY_TYPE, "Unknown entity type");
    }
    if (uchar == -1) {
        hb_proc_error(proc, HB_ERR_PARSE_INVALID_ENTITY, "Invalid entity");
    }

    if (proc->cfg->decode_entities) {
        hb_proc_write_utf_8(proc, uchar);
    } else {
        hb_proc_write_view(proc, &entity);
    }

    return uchar;
}
