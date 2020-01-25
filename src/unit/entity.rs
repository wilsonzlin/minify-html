use std::char::from_u32;

use crate::err::ProcessingResult;
use crate::proc::{Processor, ProcessorRange};
use crate::proc::MatchAction::*;
use crate::proc::MatchCond::*;
use crate::proc::MatchMode::*;
use crate::spec::codepoint::{is_digit, is_hex_digit, is_lower_hex_digit, is_upper_hex_digit};

// Some entities are actually shorter than their decoded characters as UTF-8.
// See `build.rs` for more details.

// Based on the data sourced from https://html.spec.whatwg.org/entities.json:
// - Entity names can have [A-Za-z0-9] characters, and are case sensitive.
// - Some character entity references do not end with a semicolon.
//   - All of these entities also have a corresponding entity with semicolon.
// - The longest name is "CounterClockwiseContourIntegral", with length 31
// (excluding leading ampersand and trailing semicolon).
// - All entity names are at least 2 characters long.

// Browser implementation behaviour to consider:
// - Browsers match longest sequence of characters that would form a valid entity.
// - Names must match case sensitively.
// - Entities that don't have a semicolon do work e.g. `&amp2` => `&2`.

include!(concat!(env!("OUT_DIR"), "/gen_entities.rs"));

pub fn is_valid_entity_reference_name_char(c: u8) -> bool {
    c >= b'0' && c <= b'9' || c >= b'A' && c <= b'Z' || c >= b'a' && c <= b'z'
}

#[derive(Clone, Copy)]
pub enum EntityType {
    NonDecodableRightChevron(ProcessorRange),
    Malformed(ProcessorRange),
    Ascii(u8),
    // If named or numeric reference refers to ASCII char, Type::Ascii is used instead.
    Named(&'static [u8]),
    InvalidNumeric,
    Numeric(char),
}

impl EntityType {
    pub fn keep(self, proc: &mut Processor) -> () {
        match self {
            EntityType::NonDecodableRightChevron(r) => { proc.write_range(r); }
            EntityType::Malformed(r) => { proc.write_range(r); }
            EntityType::Ascii(c) => { proc.write(c); }
            EntityType::Named(s) => { proc.write_slice(s); }
            EntityType::InvalidNumeric => { proc.write_utf8('\u{FFFD}'); }
            EntityType::Numeric(c) => { proc.write_utf8(c); }
        };
    }
}

fn parse_numeric(proc: &mut Processor, skip_amount: usize, max_len: usize, digit_pred: fn(u8) -> bool, on_digit: fn(u32, u8) -> u32) -> Option<EntityType> {
    // Skip '#' or '#x'.
    proc.skip_amount_expect(skip_amount);
    // This is required because leading zeros do not count towards digit limit.
    let has_leading_zeros = proc.m(While, Char(b'0'), Discard).nonempty();
    // Browser actually consumes unlimited amount of digits, but decodes to 0xFFFD if not a valid Unicode Scalar Value.
    // UnintentionalEntityState (UES) encodes leading ampersand in any sequence matching /&#x?\d/. This means that we need to be careful in keeping malformed behaviour consistent between this function and UES methods.
    // For example, if we simply output the entity literally, it will be interpreted as an unintentional entity by UEP and cause the written output to be shifted down to make room for inserting `amp`, which could lead to overwriting source code. This is because this function considers the entity as malformed whereas UEP doesn't and encodes the `&`.
    // Currently, since browsers decode to a replacement character (U+FFFD) if malformed, we'll simply decode to that, which won't trigger any UEP encoding behaviour.
    let raw = proc.m(While, Pred(digit_pred), Discard);
    // Semicolon is required by spec but seems to be optional in actual browser behaviour.
    proc.m(Is, Char(b';'), Discard);
    // `&` or `&#` without any digits are simply treated literally in browsers.
    if raw.len() < 1 {
        if has_leading_zeros {
            Some(EntityType::Ascii(b'\0'))
        } else {
            None
        }
    } else if raw.len() > max_len {
        Some(EntityType::InvalidNumeric)
    } else {
        let mut val = 0u32;
        for c in &proc[raw] {
            val = on_digit(val, *c);
        };
        Some(from_u32(val)
            .map(|c| if c.is_ascii() {
                EntityType::Ascii(c as u8)
            } else {
                EntityType::Numeric(c)
            })
            .unwrap_or(EntityType::InvalidNumeric))
    }
}

fn parse_name(proc: &mut Processor) -> Option<EntityType> {
    let decoded = proc.trie(ENTITY_REFERENCES);
    // In UTF-8, one-byte character encodings are always ASCII.
    decoded.map(|(r, s)| {
        proc.skip_amount_expect(r.len());
        if s.len() == 1 {
            EntityType::Ascii(s[0])
        } else {
            EntityType::Named(s)
        }
    })
}

// This will parse and skip characters.
pub fn parse_entity(proc: &mut Processor, decode_left_chevron: bool) -> ProcessingResult<EntityType> {
    let checkpoint = proc.checkpoint();
    proc.m(Is, Char(b'&'), Discard).expect();

    // The input can end at any time after initial ampersand.
    // Examples of valid complete source code: "&", "&a", "&#", "&#09",
    // "&amp".

    // These functions do not return EntityType::Malformed as it requires a checkpoint.
    // Instead, they return None if entity is malformed.
    let entity_type = match proc.peek_offset_eof(0) {
        Some(b'#') => match proc.peek_offset_eof(1) {
            Some(b'x') => parse_numeric(proc, 2, 6, is_hex_digit, |val, c| val * 16 + match c {
                c if is_digit(c) => c - b'0',
                c if is_upper_hex_digit(c) => c - b'A' + 10,
                c if is_lower_hex_digit(c) => c - b'a' + 10,
                _ => unreachable!(),
            } as u32),
            _ => parse_numeric(proc, 1, 7, is_digit, |val, c| val * 10 + (c - b'0') as u32),
        },
        _ => parse_name(proc),
    }.map(|e| match (decode_left_chevron, e) {
        (false, EntityType::Ascii(b'<')) => EntityType::NonDecodableRightChevron(proc.consumed_range(checkpoint)),
        (_, e) => e,
    }).unwrap_or_else(|| EntityType::Malformed(proc.consumed_range(checkpoint)));

    Ok(entity_type)
}
