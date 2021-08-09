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

use crate::common::gen::codepoints::{
    Lookup, ALPHANUMERIC_OR_EQUALS, DIGIT, HEX_DIGIT, LOWER_HEX_ALPHA, UPPER_HEX_ALPHA,
};
use crate::common::gen::entities::{EntityType, ENTITY};
use crate::common::pattern::TrieNodeMatch;
use crate::proc::Processor;

enum Parsed {
    // This includes numeric entities that were invalid and decoded to 0xFFFD.
    Decoded { read_len: usize, write_len: usize },
    // Some entities are shorter than their decoded UTF-8 sequence. As such, we leave them encoded.
    // Also, named entities that don't end in ';' but are followed by an alphanumeric or `=` char
    // in attribute values are also not decoded due to the spec. (See parser below for more details.)
    LeftEncoded,
    // This is for any entity-like sequence that couldn't match the `ENTITY` trie.
    Invalid { len: usize },
}

#[inline(always)]
fn parse_numeric_entity(
    code: &mut [u8],
    read_start: usize,
    prefix_len: usize,
    write_pos: usize,
    digit_lookup: &'static Lookup,
    on_digit: fn(u32, u8) -> u32,
    max_digits: usize,
) -> Parsed {
    let mut value = 0u32;
    let mut digits = 0;
    let mut read_next = read_start + prefix_len;
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
        .and_then(|v| from_u32(v))
        .unwrap_or('\u{FFFD}');
    Parsed::Decoded {
        read_len: read_next - read_start,
        write_len: char.encode_utf8(&mut code[write_pos..]).len(),
    }
}

// Parse the entity and write its decoded value at {@param write_pos}.
// If malformed, returns the longest matching entity prefix length, and does not write/decode anything.
fn parse_entity(code: &mut [u8], read_pos: usize, write_pos: usize, in_attr_val: bool) -> Parsed {
    match ENTITY.longest_matching_prefix(&code[read_pos..]) {
        TrieNodeMatch::Found {
            len: match_len,
            value,
        } => match value {
            EntityType::Dec => parse_numeric_entity(
                code,
                read_pos,
                // Skip past '&#'. Note that match_len is 3 as it matches '&#[0-9]'.
                2,
                write_pos,
                DIGIT,
                |value, c| value.wrapping_mul(10).wrapping_add((c - b'0') as u32),
                7,
            ),
            EntityType::Hex => parse_numeric_entity(
                code,
                read_pos,
                // Skip past '&#x'. Note that match_len is 4 as it matches '&#x[0-9a-fA-F]'.
                3,
                write_pos,
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
                // https://html.spec.whatwg.org/multipage/parsing.html#named-character-reference-state.
                if decoded.len() > match_len
                    || in_attr_val
                        && *code.get(read_pos + match_len - 1).unwrap() != b';'
                        && code
                            .get(read_pos + match_len)
                            .filter(|c| ALPHANUMERIC_OR_EQUALS[**c])
                            .is_some()
                {
                    Parsed::LeftEncoded
                } else {
                    code[write_pos..write_pos + decoded.len()].copy_from_slice(decoded);
                    Parsed::Decoded {
                        read_len: match_len,
                        write_len: decoded.len(),
                    }
                }
            }
        },
        // The entity is malformed.
        TrieNodeMatch::NotFound { reached } => Parsed::Invalid { len: reached },
    }
}

// Normalise entity such that "&lt; hello" becomes "___< hello".
// For something like "&a&#109;&#112; hello", it becomes "_______&ampamp hello".
pub fn maybe_normalise_entity(proc: &mut Processor, in_attr_val: bool) -> bool {
    if proc.peek(0).filter(|c| *c == b'&').is_none() {
        return false;
    };

    let start = proc.read_next;

    // We want to look ahead in case this entity decodes to something beginning with '&' and the following code (after
    // any decoding) would form an unintentional entity.
    // For example, `&a&#109p;` would output as `&amp`, which is an unintentional entity.
    let mut read_next = start;
    let mut write_next = start;
    let mut node = ENTITY;
    while node.value.is_none() {
        match proc.code.get(read_next) {
            None => break,
            Some(b'&') => {
                // Decode before checking to see if it continues current entity.
                let (read_len, write_len) =
                    match parse_entity(proc.code, read_next, write_next, in_attr_val) {
                        Parsed::LeftEncoded => {
                            // Don't mistake an intentionally undecoded entity for an unintentional entity.
                            break;
                        }
                        Parsed::Decoded {
                            read_len,
                            write_len,
                        } => {
                            debug_assert!(read_len > 0);
                            debug_assert!(write_len > 0);
                            (read_len, write_len)
                        }
                        Parsed::Invalid { len } => {
                            debug_assert!(len > 0);
                            // We only want to keep reading entities that will decode. No entity has an ampersand after the
                            // first character, so we don't need to keep checking if we see one; however, malformed entities
                            // could be part of their own unintentional entity, so don't consume them.
                            //
                            // For example:
                            // &am&am&#112;
                            // When parsing from the first `&`, stop before the second `&`, as otherwise the second `&am`
                            // won't be normalised to `&ampamp;`.
                            if read_next != start {
                                break;
                            };
                            proc.code
                                .copy_within(read_next..read_next + len, write_next);
                            (len, len)
                        }
                    };
                debug_assert!(read_len > 0);

                let (new_node, match_len) = node
                    .shortest_matching_prefix(&proc.code[write_next..write_next + write_len], 0);
                node = new_node;
                read_next += read_len;
                write_next += write_len;
                if match_len < write_len {
                    // Either new_node has a value, or we can't match anymore and so there will definitely be no
                    // unintentional entity.
                    break;
                };
            }
            Some(_) => {
                let (new_node, new_read_next) =
                    node.shortest_matching_prefix(&proc.code, read_next);
                let len = new_read_next - read_next;
                if len == 0 {
                    break;
                };
                proc.code.copy_within(read_next..new_read_next, write_next);
                read_next += len;
                write_next += len;
                node = new_node;
            }
        };
    }
    // Check if we need to encode initial '&' and add 'amp'.
    let undecodable = node.value.is_some();
    // Shift decoded value down so that it ends at read_next (exclusive).
    let mut shifted_start = read_next - (write_next - start - undecodable as usize);
    proc.code
        .copy_within(start + undecodable as usize..write_next, shifted_start);
    if undecodable {
        debug_assert_eq!(proc.code.get(start), Some(&b'&'));
        proc.code[shifted_start - 4..shifted_start].copy_from_slice(b"&amp");
        shifted_start -= 4;
    };

    proc.read_next = shifted_start;
    return true;
}
