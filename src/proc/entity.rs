use crate::gen::entities::{ENTITY, EntityType};
use crate::pattern::TrieNodeMatch;
use std::char::from_u32;
use crate::spec::codepoint::{is_hex_digit, is_digit, is_lower_hex_digit, is_upper_hex_digit};
use crate::proc::Processor;

#[inline(always)]
fn parse_numeric_entity(code: &mut [u8], read_start: usize, write_pos: usize, is_digit: fn(u8) -> bool, on_digit: fn(u32, u8) -> u32, max_digits: u8) -> (usize, usize) {
    let mut value = 0u32;
    let mut digits = 0;
    let mut read_next = read_start;
    // Skip initial zeros.
    while code.get(read_next).filter(|c| **c == b'0').is_some() {
        read_next += 1;
    };
    // Browser will still continue to consume digits past max_digits.
    loop {
        match code.get(read_next) {
            Some(&c) if is_digit(c) => {
                // We don't care about overflow, as it will be considered malformed past max_digits anyway.
                value = on_digit(value, c);
                read_next += 1;
                digits += 1;
            }
            _ => break,
        };
    };
    // Semicolon is required by spec but seems to be optional in actual browser behaviour.
    if let Some(b';') = code.get(read_next) {
        read_next += 1;
    };
    // Browsers decode to a replacement character (U+FFFD) if malformed.
    let char = Some(value)
        .filter(|_| digits <= max_digits)
        .and_then(|v| from_u32(v))
        .unwrap_or('\u{FFFD}');
    (read_next - read_start, char.encode_utf8(&mut code[write_pos..]).len())
}

// Parse the entity and write its decoded value at the beginning of {@param code}.
// Return the (read_len, write_len).
// If malformed, returns the longest matching entity prefix length as (len, len).
fn parse_entity(code: &mut [u8], read_pos: usize, write_pos: usize) -> (usize, usize) {
    match ENTITY.longest_matching_prefix(&code[read_pos..]) {
        TrieNodeMatch::Found { len: match_len, value } => match value {
            EntityType::Dec => parse_numeric_entity(
                code,
                // Skip past '&#'. Note that match_len is 3 as it matches '&#[0-9]'.
                read_pos + 2,
                write_pos,
                is_digit,
                |value, c| value.wrapping_mul(10).wrapping_add((c - b'0') as u32),
                7,
            ),
            EntityType::Hex => parse_numeric_entity(
                code,
                // Skip past '&#x'. Note that match_len is 4 as it matches '&#x[0-9a-fA-F]'.
                read_pos + 3,
                write_pos,
                is_hex_digit,
                |value, c| value.wrapping_mul(16).wrapping_add(match c {
                    c if is_digit(c) => (c - b'0') as u32,
                    c if is_lower_hex_digit(c) => (c - b'a') as u32,
                    c if is_upper_hex_digit(c) => (c - b'A') as u32,
                    _ => unreachable!(),
                }),
                6,
            ),
            EntityType::Named(decoded) => {
                code[write_pos..write_pos + decoded.len()].copy_from_slice(decoded);
                (match_len, decoded.len())
            }
        },
        // The entity is malformed.
        TrieNodeMatch::NotFound { reached } => (reached, reached),
    }
}

// Normalise entity such that "&lt; hello" becomes "___< hello" and the range of '<' is returned.
// For something like "&a&#109;&#112; hello", it becomes "_______&ampamp hello" and (7, 14) is returned.
pub fn maybe_normalise_entity(proc: &mut Processor) -> usize {
    if proc.peek(0).filter(|c| *c == b'&').is_none() {
        return 0;
    };

    let start = proc.read_next;

    // We want to look ahead in case this entity decodes to something beginning with '&' and following code are also
    // entities that would decode to form an unintentional entity once decoded.
    // For example, `&am&#113;` would output as `&amp` which is an unintentional entity.
    let mut read_next = start;
    let mut write_next = start;
    let mut node = Some(ENTITY);
    while node.filter(|n| n.value.is_none()).is_some()
        && proc.code.get(read_next).filter(|c| **c == b'&').is_some()
    {
        let (entity_read, entity_write) = parse_entity(proc.code, read_next, write_next);

        node = node.unwrap().next_matching_node(&proc.code[write_next..write_next + entity_write], 0).map(|(node, _)| node);
        debug_assert!(entity_read > 0);
        read_next += entity_read;
        write_next += entity_write;
    };
    // Need to encode initial '&', so add 'amp'.
    let undecodable = node.and_then(|n| n.value).is_some();
    // Shift decoded value down so that it ends at read_next (exclusive).
    let mut shifted_start = read_next - (write_next - start - undecodable as usize);
    proc.code.copy_within(start + undecodable as usize..write_next, shifted_start);
    if undecodable {
        debug_assert_eq!(proc.code.get(start), Some(&b'&'));
        proc.code[shifted_start - 4..shifted_start].copy_from_slice(b"&amp");
        shifted_start -= 4;
    };

    proc.read_next = shifted_start;
    return read_next - shifted_start;
}
