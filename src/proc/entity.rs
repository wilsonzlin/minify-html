// The minimum length of any entity is 3, which is a character entity reference
// with a single character name. The longest UTF-8 representation of a Unicode
// code point is 4 bytes. Because there are no character entity references with
// a name of length 1, it's always better to decode entities for minification
// purposes.

// Based on the data sourced from https://www.w3.org/TR/html5/entities.json as
// of 2019-04-20T04:00:00.000Z:
// - Entity names can have [A-Za-z0-9] characters, and are case sensitive.
// - Some character entity references do not need to end with a semicolon.
// - The longest name is "CounterClockwiseContourIntegral", with length 31
// (excluding leading ampersand and trailing semicolon).
// - All entity names are at least 2 characters long.

// Browser implementation behaviour to consider:
// - It is unclear what happens if an entity name does not match case
// sensitively but matches two or more case insensitively.
//   - For example, given "AlphA" or "aLpha", does the browser choose "alpha" or
//   "Alpha"?
// - Do browsers render valid entities without trailing semicolons?
//   - For example, how do browsers interpret "Chuck-&amp-Cheese", "1&amp1", and
//   "&ampe;"?

// hyperbuild implementation:
// - Entities must start with an ampersand and end with a semicolon.
// - Once an ampersand is encountered, it and the sequence of characters
// following must match the following ECMAScript regular expression to be
// considered a well formed entity:
//
//   /&(#(x[0-9a-f]{1-6}|[0-9]{1,7}))|[a-z0-9]{2,31};/i
//
// - If the sequence of characters following an ampersand do not combine to form
// a well formed entity, the ampersand is considered a bare ampersand.
//   - A bare ampersand is an ampersand that is interpreted literally and not as
//   the start of an entity.
//   - hyperbuild looks ahead without consuming to check if the following
//   characters would form a well formed entity. If they don't, only the longest
//   subsequence that could form a well formed entity is consumed.
// - An entity is considered invalid if it is well formed but represents a
// non-existent Unicode code point or reference name.

use crate::proc::Processor;
use crate::spec::codepoint::{is_digit, is_upper_hex_digit, is_lower_hex_digit, is_hex_digit};
use crate::spec::entity::{ENTITY_REFERENCES, is_valid_entity_reference_name_char};
use crate::err::HbRes;
use crate::code::Code;

const MAX_UNICODE_CODE_POINT: u32 = 0x10FFFF;

enum Type {
    Malformed,
    Name,
    Decimal,
    Hexadecimal,
}

fn parse_decimal(slice: &[u8]) -> Option<u32> {
    let mut val = 0u32;
    for c in slice {
        val = val * 10 + (c - b'0');
    }
    if val > MAX_UNICODE_CODE_POINT {
        None
    } else {
        val
    }
}

fn parse_hexadecimal(slice: &[u8]) -> Option<u32> {
    let mut val = 0u32;
    for c in slice {
        let digit: u32 = if is_digit(c) {
            c - b'0'
        } else if is_upper_hex_digit(c) {
            c - b'A' + 10
        } else if is_lower_hex_digit(c) {
            c - b'a' + 10
        } else {
            unreachable!();
        };
        val = val * 16 + digit;
    }
    if val > MAX_UNICODE_CODE_POINT {
        None
    } else {
        val
    }
}

// This will parse and skip characters. Set a checkpoint to later write skipped, or to ignore results and reset to previous position.
pub fn parse_entity<D: Code>(proc: &Processor<D>) -> HbRes<Option<u32>> {
    proc.match_char(b'&').expect().discard();

    // The input can end at any time after initial ampersand.
    // Examples of valid complete source code: "&", "&a", "&#", "&#09",
    // "&amp".

    // There are three stages to this function:
    //
    // 1. Determine the type of entity, so we can know how to parse and
    // validate the following characters.
    //    - This can be done by simply looking at the first and second
    //    characters after the initial ampersand, e.g. "&#", "&#x", "&a".
    // 2. Parse the entity data, i.e. the characters between the ampersand
    // and semicolon.
    //    - To avoid parsing forever on malformed entities without
    //    semicolons, there is an upper bound on the amount of possible
    //    characters, based on the type of entity detected from the first
    //    stage.
    // 3. Interpret and validate the data.
    //    - This simply checks if it refers to a valid Unicode code point or
    //    entity reference name.

    // First stage: determine the type of entity.
    let predicate: fn(u8) -> bool;
    let entity_type: Type;
    let min_len: usize;
    let max_len: usize;

    if proc.match_seq(b"#x").discard().matched() {
        predicate = is_hex_digit;
        entity_type = Type::Hexadecimal;
        min_len = 1;
        max_len = 6;
    } else if proc.match_char(b'#').discard().matched() {
        predicate = is_digit;
        entity_type = Type::Decimal;
        min_len = 1;
        max_len = 7;
    } else if proc.match_pred(is_valid_entity_reference_name_char).matched() {
        predicate = is_valid_entity_reference_name_char;
        entity_type = Type::Name;
        min_len = 2;
        max_len = 31;
    } else {
        return Ok(None);
    }

    // Second stage: try to parse a well formed entity.
    // Malformed entity could be last few characters in code, so allow EOF during entity.
    let data = proc.match_while_pred(predicate).discard().slice();
    if data.len() < min_len || data.len() > max_len {
        entity_type = Type::Malformed;
    };
    // Don't try to consume semicolon if entity is not well formed already.
    if entity_type != Type::Malformed && !proc.match_char(b';').discard().matched() {
        entity_type = Type::Malformed;
    };

    // Third stage: validate entity and decode if configured to do so.
    Ok(match entity_type {
        Type::Name => ENTITY_REFERENCES.get(data).map(|r| *r),
        Type::Decimal => parse_decimal(data),
        Type::Hexadecimal => parse_hexadecimal(data),
        Type::Malformed => None,
    })
}

/**
 * Process an HTML entity.
 *
 * @return Unicode code point of the entity, or HB_UNIT_ENTITY_NONE if the
 * entity is malformed or invalid
 */
pub fn process_entity<D: Code>(proc: &Processor<D>) -> HbRes<Option<u32>> {
    let checkpoint = proc.checkpoint();
    let parsed = parse_entity(proc)?;

    if let Some(cp) = parsed {
        proc.write_utf8(cp);
    } else {
        // Write discarded characters that could not form a well formed entity.
        checkpoint.write_skipped();
    };

    Ok(parsed)
}
