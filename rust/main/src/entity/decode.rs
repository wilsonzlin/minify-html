// Based on the data sourced from https://html.spec.whatwg.org/entities.json:
// - Entity names can have [A-Za-z0-9] characters, and are case sensitive.
// - Some character entity references do not end with a semicolon.
//   - All of these entities also have a corresponding entity with semicolon.
// - The longest name is "CounterClockwiseContourIntegral", with length 31 (excluding leading ampersand and trailing
//   semicolon).
// - All entity names are at least 2 characters long.
// - Some named entities are actually shorter than their decoded characters as UTF-8.

// Browser implementation behaviour to consider:
// - Browsers match longest sequence of characters that would form a valid entity.
// - Names must match case sensitively.
// - For a numeric entity, browsers actually consume an unlimited amount of digits, but decode to 0xFFFD if not a valid
//   Unicode Scalar Value.

use std::char::from_u32;

use memchr::memchr;

use crate::common::gen::codepoints::{
    Lookup, ALPHANUMERIC_OR_EQUALS, DIGIT, HEX_DIGIT, LOWER_HEX_ALPHA, UPPER_HEX_ALPHA,
};
use crate::common::gen::entities::{EntityType, ENTITY};
use crate::common::pattern::TrieNodeMatch;

enum Decoded {
    Ignored,
    Named(&'static [u8]),
    Numeric(char),
}

struct ParsedEntity {
    decoded: Decoded,
    read_len: usize,
}

fn parse_numeric_entity(
    code: &[u8],
    // read_start should be separate (and not simply `&code[read_start..]`) so that read_len result is correct.
    read_start: usize,
    digit_lookup: &'static Lookup,
    on_digit: fn(u32, u8) -> u32,
    max_digits: usize,
) -> ParsedEntity {
    let mut value = 0u32;
    let mut digits = 0;
    let mut read_next = read_start;
    // Skip initial zeros.
    while code.get(read_next).filter(|c| **c == b'0').is_some() {
        read_next += 1;
    }
    // Browser will still continue to consume digits past max_digits.
    loop {
        match code.get(read_next) {
            Some(&c) if digit_lookup[c] => {
                // We don't care about overflow, as it will be considered malformed past max_digits anyway.
                value = on_digit(value, c);
                read_next += 1;
                digits += 1;
            }
            _ => break,
        };
    }
    // Semicolon is required by spec but seems to be optional in actual browser behaviour.
    if let Some(b';') = code.get(read_next) {
        read_next += 1;
    };
    // Browsers decode to a replacement character (U+FFFD) if malformed.
    let char = Some(value)
        .filter(|_| digits <= max_digits)
        .and_then(from_u32)
        .unwrap_or('\u{FFFD}');
    ParsedEntity {
        read_len: read_next,
        decoded: Decoded::Numeric(char),
    }
}

fn parse_entity(code: &[u8], in_attr_val: bool) -> ParsedEntity {
    match ENTITY.longest_matching_prefix(code) {
        // The entity is malformed.
        TrieNodeMatch::NotFound { reached } => ParsedEntity {
            read_len: reached,
            decoded: Decoded::Ignored,
        },
        TrieNodeMatch::Found {
            len: match_len,
            value,
        } => match value {
            EntityType::Dec => parse_numeric_entity(
                code,
                // Skip past '&#'. Note that match_len is 3 as it matches '&#[0-9]'.
                2,
                DIGIT,
                |value, c| value.wrapping_mul(10).wrapping_add((c - b'0') as u32),
                7,
            ),
            EntityType::Hex => parse_numeric_entity(
                code,
                // Skip past '&#x'. Note that match_len is 4 as it matches '&#x[0-9a-fA-F]'.
                3,
                HEX_DIGIT,
                |value, c| {
                    value.wrapping_mul(16).wrapping_add(match c {
                        c if DIGIT[c] => (c - b'0') as u32,
                        c if LOWER_HEX_ALPHA[c] => 10 + (c - b'a') as u32,
                        c if UPPER_HEX_ALPHA[c] => 10 + (c - b'A') as u32,
                        _ => unreachable!(),
                    })
                },
                6,
            ),
            EntityType::Named(decoded) => {
                if in_attr_val
                    && code[match_len - 1] != b';'
                    && code
                        .get(match_len)
                        .filter(|&&c| ALPHANUMERIC_OR_EQUALS[c])
                        .is_some()
                {
                    // Don't decode if named entity is inside an attribute value and doesn't end with semicolon but is followed by an alphanumeric or `=` character.
                    // https://html.spec.whatwg.org/multipage/parsing.html#named-character-reference-state.
                    ParsedEntity {
                        read_len: match_len,
                        decoded: Decoded::Ignored,
                    }
                } else {
                    // NOTE: `decoded` might be in encoded form if encoded form is shorter than decoded.
                    ParsedEntity {
                        read_len: match_len,
                        decoded: Decoded::Named(decoded),
                    }
                }
            }
        },
    }
}

pub fn decode_entities(mut code: &[u8], in_attr_val: bool) -> Vec<u8> {
    let mut res = Vec::<u8>::new();
    while !code.is_empty() {
        let (before, matched) = match memchr(b'&', code) {
            None => (code.len(), false),
            Some(n) => (n, true),
        };
        res.extend_from_slice(&code[..before]);
        code = &code[before..];
        if matched {
            let ParsedEntity { decoded, read_len } = parse_entity(code, in_attr_val);
            match decoded {
                Decoded::Numeric(c) => {
                    let mut buf = [0u8; 4];
                    let encoded = c.encode_utf8(&mut buf);
                    res.extend_from_slice(encoded.as_bytes());
                }
                Decoded::Ignored => res.extend_from_slice(&code[..read_len]),
                Decoded::Named(s) => res.extend_from_slice(s),
            };
            code = &code[read_len..];
        };
    }
    res
}
