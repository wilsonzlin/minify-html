use phf::phf_map;

use crate::err::ProcessingResult;
use crate::pattern::TrieNode;
use crate::proc::{Processor, ProcessorRange};
use crate::spec::codepoint::{is_digit, is_hex_digit, is_lower_hex_digit, is_upper_hex_digit};
use crate::ErrorType;

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

fn is_valid_entity_reference_name_char(c: u8) -> bool {
    c >= b'0' && c <= b'9' || c >= b'A' && c <= b'Z' || c >= b'a' && c <= b'z'
}

#[derive(Clone, Copy)]
pub enum EntityType {
    NonDecodable(ProcessorRange),
    Malformed(ProcessorRange),
    Ascii(u8),
    // If named or numeric reference refers to ASCII char, Type::Ascii is used instead.
    Named(&'static [u8]),
    Numeric(char),
}

impl EntityType {
    pub fn is_malformed(&self) -> bool {
        if let EntityType::Malformed(_) = self {
            true
        } else {
            false
        }
    }
}

impl EntityType {
    pub fn keep(self, proc: &mut Processor) -> () {
        match self {
            EntityType::NonDecodable(r) => proc.write_range(r),
            EntityType::Malformed(r) => proc.write_range(r),
            EntityType::Ascii(c) => proc.write(c),
            EntityType::Named(s) => proc.write_slice(s),
            EntityType::Numeric(c) => proc.write_utf8(c),
        };
    }
}

macro_rules! handle_decoded_numeric_code_point {
    ($proc:ident, $at_least_one_digit:ident, $code_point:ident) => {
        if !$at_least_one_digit || !chain!($proc.match_char(b';').discard().matched()) {
            return None;
        }
        return std::char::from_u32($code_point).map(|c| if c.is_ascii() {
            EntityType::Ascii(c as u8)
        } else {
            EntityType::Numeric(c)
        });
    };
}

fn parse_decimal(proc: &mut Processor) -> Option<EntityType> {
    let mut val = 0u32;
    let mut at_least_one_digit = false;
    // Parse at most seven characters to prevent parsing forever and overflowing.
    for _ in 0..7 {
        if let Some(c) = chain!(proc.match_pred(is_digit).discard().maybe_char()) {
            at_least_one_digit = true;
            val = val * 10 + (c - b'0') as u32;
        } else {
            break;
        }
    };
    handle_decoded_numeric_code_point!(proc, at_least_one_digit, val);
}

fn parse_hexadecimal(proc: &mut Processor) -> Option<EntityType> {
    let mut val = 0u32;
    let mut at_least_one_digit = false;
    // Parse at most six characters to prevent parsing forever and overflowing.
    for _ in 0..6 {
        if let Some(c) = chain!(proc.match_pred(is_hex_digit).discard().maybe_char()) {
            at_least_one_digit = true;
            let digit = if is_digit(c) {
                c - b'0'
            } else if is_upper_hex_digit(c) {
                c - b'A' + 10
            } else if is_lower_hex_digit(c) {
                c - b'a' + 10
            } else {
                unreachable!();
            };
            val = val * 16 + digit as u32;
        } else {
            break;
        }
    };
    handle_decoded_numeric_code_point!(proc, at_least_one_digit, val);
}

fn parse_name(proc: &mut Processor) -> Option<EntityType> {
    // In UTF-8, one-byte character encodings are always ASCII.
    ENTITY_REFERENCES.get(proc).map(|s| if s.len() == 1 {
        EntityType::Ascii(s[0])
    } else {
        EntityType::Named(s)
    })
}

// TODO Decoding '<' in content.
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
    let entity_type = if chain!(proc.match_seq(b"#x").discard().matched()) {
        parse_hexadecimal(proc)
    } else if chain!(proc.match_char(b'#').discard().matched()) {
        parse_decimal(proc)
    } else if chain!(proc.match_pred(is_valid_entity_reference_name_char).matched()) {
        parse_name(proc)
    } else {
        // At this point, only consumed ampersand.
        None
    }
        .map(|e| match (decode_left_chevron, e) {
            (_, EntityType::Ascii(b'&')) | (false, EntityType::Ascii(b'<')) => EntityType::NonDecodable(proc.consumed_range(checkpoint)),
            (_, e) => e,
        })
        .unwrap_or_else(|| EntityType::Malformed(proc.consumed_range(checkpoint)));

    if entity_type.is_malformed() && chain!(proc.match_char(b'&').matched()) {
        Err(ErrorType::EntityFollowingMalformedEntity)
    } else {
        Ok(entity_type)
    }
}
