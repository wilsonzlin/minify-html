use std::char::from_u32;

use crate::err::ProcessingResult;
use crate::proc::{Processor, ProcessorRange};
use crate::spec::codepoint::{is_digit, is_lower_hex_digit, is_upper_hex_digit};

// The minimum length of any entity is 3, which is a character entity reference
// with a single character name. The longest UTF-8 representation of a Unicode
// code point is 4 bytes. Because there are no character entity references with
// a name of length 1, it's always better to decode entities for minification
// purposes.

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
    Numeric(char),
}

impl EntityType {
    pub fn keep(self, proc: &mut Processor) -> () {
        match self {
            EntityType::NonDecodableRightChevron(r) => { proc.write_range(r); }
            EntityType::Malformed(r) => { proc.write_range(r); }
            EntityType::Ascii(c) => { proc.write(c); }
            EntityType::Named(s) => { proc.write_slice(s); }
            EntityType::Numeric(c) => { proc.write_utf8(c); }
        };
    }
}

fn handle_decoded_numeric_code_point(proc: &mut Processor, digits: usize, code_point: u32) -> Option<EntityType> {
    proc.skip_amount_expect(digits);
    if digits == 0 {
        None
    } else {
        // Semicolon is required by spec but seems to be optional in actual browser behaviour.
        chain!(proc.match_char(b';').discard());
        from_u32(code_point).map(|c| if c.is_ascii() {
            EntityType::Ascii(c as u8)
        } else {
            EntityType::Numeric(c)
        })
    }
}

fn parse_decimal(proc: &mut Processor) -> Option<EntityType> {
    // Skip '#'.
    proc.skip_amount_expect(1);
    let mut val = 0u32;
    let mut i = 0;
    // TODO Browser actually consumes unlimited chars but replaces with 0xFFFD if invalid.
    // Parse at most seven characters to prevent parsing forever and overflowing.
    while i < 7 {
        match proc.peek_offset_eof(i) {
            Some(c) if is_digit(c) => val = val * 10 + (c - b'0') as u32,
            _ => break,
        };
        i += 1;
    };
    handle_decoded_numeric_code_point(proc, i, val)
}

fn parse_hexadecimal(proc: &mut Processor) -> Option<EntityType> {
    // Skip '#x'.
    proc.skip_amount_expect(2);
    let mut val = 0u32;
    let mut i = 0;
    // TODO Browser actually consumes unlimited chars but replaces with 0xFFFD if invalid.
    // Parse at most six characters to prevent parsing forever and overflowing.
    while i < 6 {
        let digit = match proc.peek_offset_eof(i) {
            Some(c) if is_digit(c) => c - b'0',
            Some(c) if is_upper_hex_digit(c) => c - b'A' + 10,
            Some(c) if is_lower_hex_digit(c) => c - b'a' + 10,
            _ => break,
        };
        val = val * 16 + digit as u32;
        i += 1;
    };
    handle_decoded_numeric_code_point(proc, i, val)
}

fn parse_name(proc: &mut Processor) -> Option<EntityType> {
    // In UTF-8, one-byte character encodings are always ASCII.
    let decoded = proc.match_trie(ENTITY_REFERENCES);
    proc.discard();
    decoded.map(|s| if s.len() == 1 {
        EntityType::Ascii(s[0])
    } else {
        EntityType::Named(s)
    })
}

// This will parse and skip characters.
// Issues:
// - Malformed entities including bare ampersand could form valid entity if there are immediately following valid entities which are decoded.
// Notes:
// - To prevent an entity from being interpreted as one, one of its characters ([&#a-zA-Z0-9;]) needs to be encoded. Ampersand is the shortest, even with semicolon (`&amp` or `&amp;`).
// Solution:
// - Disallow following malformed entities with ampersand.
// - Do not decode encoded ampersand (e.g. `&AMP` or `&#x26;`) to prevent accidentally writing entity.
pub fn parse_entity(proc: &mut Processor, decode_left_chevron: bool) -> ProcessingResult<EntityType> {
    let checkpoint = proc.checkpoint();
    if cfg!(debug_assertions) {
        chain!(proc.match_char(b'&').expect().discard());
    } else {
        proc.skip_expect();
    };

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

    // These functions do not return EntityType::Malformed as it requires a checkpoint.
    // Instead, they return None if entity is malformed.
    let entity_type = match proc.peek_offset_eof(0) {
        Some(b'#') => match proc.peek_offset_eof(1) {
            Some(b'x') => parse_hexadecimal(proc),
            _ => parse_decimal(proc),
        },
        _ => parse_name(proc),
    }
        .map(|e| match (decode_left_chevron, e) {
            (false, EntityType::Ascii(b'<')) => EntityType::NonDecodableRightChevron(proc.consumed_range(checkpoint)),
            (_, e) => e,
        })
        .unwrap_or_else(|| EntityType::Malformed(proc.consumed_range(checkpoint)));

    Ok(entity_type)
}
