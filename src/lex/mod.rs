use crate::gen::dfa::{State, TRANSITIONS, ConsumeMode};
use crate::err::{HbRes, HbErr};
use crate::pattern::{TrieNodeMatch};
use crate::spec::codepoint::{is_digit, is_hex_digit, is_lower_hex_digit, is_upper_hex_digit};
use crate::gen::entities::{EntityType, ENTITY};
use std::char::from_u32;

pub struct CodeRange {
    start: usize,
    // Exclusive.
    end: usize,
}

pub struct Token {
    state: State,
    accumulated_len: usize,
}

pub struct Lexer<'c> {
    code: &'c mut [u8],

    state_cur: State,
    prefix_len: usize,
    read_next: usize,

    write_next: usize,
    last_write_start: usize,
    last_write_state: State,
}

#[inline(always)]
fn parse_numeric_entity(code: &mut [u8], read_start: usize, is_digit: fn(u8) -> bool, on_digit: fn(u32, u8) -> u32, max_digits: u8) -> (usize, usize) {
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
    (read_next, char.encode_utf8(code).len())
}

// Parse the entity and write its decoded value at the beginning of {@param code}.
// Return the (read_len, write_len).
// If malformed, returns the longest matching entity prefix length as (len, len).
fn parse_entity(code: &mut [u8]) -> (usize, usize) {
    match ENTITY.longest_matching_prefix(code) {
        TrieNodeMatch::Found { len: match_len, value } => match value {
            EntityType::Dec => {
                parse_numeric_entity(
                    code,
                    // Skip past '&#'. Note that match_len is 3 as it matches '&#[0-9]'.
                    2,
                    is_digit,
                    |value, c| value.wrapping_mul(10).wrapping_add((c - b'0') as u32),
                    7,
                )
            }
            EntityType::Hex => {
                parse_numeric_entity(
                    code,
                    // Skip past '&#x'. Note that match_len is 4 as it matches '&#x[0-9a-fA-F]'.
                    3,
                    is_hex_digit,
                    |value, c| value.wrapping_mul(16).wrapping_add(match c {
                        c if is_digit(c) => (c - b'0') as u32,
                        c if is_lower_hex_digit(c) => (c - b'a') as u32,
                        c if is_upper_hex_digit(c) => (c - b'A') as u32,
                        _ => unreachable!(),
                    }),
                    6,
                )
            }
            EntityType::Named(decoded) => {
                code[..decoded.len()].copy_from_slice(decoded);
                (match_len, decoded.len())
            }
        },
        // The entity is malformed.
        TrieNodeMatch::NotFound { reached } => (reached, reached),
    }
}

// Normalise entity such that "&amp; hello" becomes "___& hello" and the range of '&' is returned.
// For something like "&a&#109;&#112; hello", it becomes "_______&ampamp hello" and (7, 14) is returned.
fn normalise_entity(code: &mut [u8], start: usize) -> (usize, usize) {
    debug_assert_eq!(code[start], b'&');

    // We want to look ahead in case this entity decodes to something beginning with '&' and following code are also
    // entities that would decode to form an unintentional entity once decoded.
    // For example, `&am&#113;` would output as `&amp` which is an unintentional entity.
    let mut read_next = start;
    let mut write_next = start;
    let mut node = Some(ENTITY);
    while node.filter(|n| n.value.is_none()).is_some()
        && code.get(read_next).filter(|c| **c == b'&').is_some()
    {
        let (entity_read, entity_write) = parse_entity(&mut code[read_next..]);
        node = node.unwrap().next_matching_node(&code[write_next..write_next + entity_write], 0).map(|(node, _)| node);
        read_next += entity_read;
        write_next += entity_write;
    };
    // Need to encode initial '&', so add 'amp'.
    let undecodable = node.and_then(|n| n.value).is_some();
    // Shift decoded value down so that it ends at read_next (exclusive).
    let mut shifted_start = read_next - (write_next - start - undecodable as usize);
    code.copy_within(start + undecodable as usize..write_next, shifted_start);
    if undecodable {
        debug_assert_eq!(code.get(start), Some(&b'&'));
        code[shifted_start - 4..shifted_start].copy_from_slice(b"&amp");
        shifted_start -= 4;
    };
    (shifted_start, read_next)
}

fn write_accumulated(code: &mut [u8], source: usize, end: usize, dest: usize, lowercase: bool) -> () {
    if lowercase {
        code[source..end].make_ascii_lowercase();
    };
    code.copy_within(source..end, dest);
}

impl<'c> Lexer<'c> {
    pub fn new(code: &'c mut [u8]) -> Lexer {
        Lexer {
            code,

            state_cur: State::Text,
            // Characters used to transition from previous state that are part of current state.
            prefix_len: 0,
            read_next: 0,

            last_write_state: State::Unknown,
            last_write_start: 0,
            write_next: 0,
        }
    }

    pub fn state(&self) -> State {
        self.state_cur
    }

    pub fn last_write_state(&self) -> State {
        self.last_write_state
    }

    // Lex zero or more of the next characters into a token, consuming them.
    pub fn accumulate_until_next_state(&mut self) -> HbRes<Token> {
        let mut lowercase = false;
        let mut unwritten_accumulated_start = self.read_next;
        let mut written_accumulated_len = 0;
        loop {
            let transitions = TRANSITIONS[self.state_cur as usize];
            match transitions.longest_matching_prefix(&self.code[self.read_next..]) {
                TrieNodeMatch::Found { len, value } => {
                    if value.to == self.state_cur {
                        // Accumulate characters as part of current state.
                        self.read_next += len;
                        lowercase = value.consume == ConsumeMode::AccumulateLowerCase;
                    } else {
                        // Exit state or normalise entity.
                        // Write any unwritten accumulated characters.
                        // If entity, write existing unwritten accumulated characters, so that normalised entity is
                        // written in correct order.

                        if value.consume == ConsumeMode::Current {
                            self.read_next += len;
                        };
                        write_accumulated(self.code, unwritten_accumulated_start - self.prefix_len, self.read_next, self.write_next + written_accumulated_len, lowercase);
                        written_accumulated_len += self.read_next - unwritten_accumulated_start + self.prefix_len;
                        self.prefix_len = 0;

                        if value.to == State::TextEntity || value.to == State::AttrValueEntity {
                            // These are special states manually handled by this code.
                            // They can only be arrived from Text{,Whitespace} or Attr*Value{,Whitespace}.
                            let (entity_start, entity_end) = normalise_entity(&mut self.code, self.read_next);
                            // Reset unwritten accumulated start position.
                            unwritten_accumulated_start = entity_start;
                            self.read_next = match self.code[entity_start] {
                                // If it could not be decoded, then skip over all characters (they would all be
                                // accumulated under Text/Attr*Value states anyway) so that we don't loop back into this
                                // entity state.
                                b'&' => entity_end,
                                // Allow state to lex decoded entity value.
                                _ => entity_start,
                            };
                            // Leave state as is, do not change to *Entity.
                        } else {
                            let res = Ok(Token {
                                state: self.state_cur,
                                accumulated_len: written_accumulated_len,
                            });
                            if value.consume == ConsumeMode::Next {
                                self.prefix_len = len;
                                self.read_next += len;
                            };
                            self.state_cur = value.to;
                            return res;
                        };
                    };
                }
                TrieNodeMatch::NotFound { .. } => return Err(HbErr::Unexpected(self.read_next)),
            };
        };
    }

    // Write a token. While not checked, the token must be the latest created by `.next()`, otherwise the code will
    // become corrupted.
    pub fn keep(&mut self, token: Token) -> CodeRange {
        self.last_write_state = token.state;
        self.last_write_start = self.write_next;
        self.write_next += token.accumulated_len;
        CodeRange { start: self.last_write_start, end: self.write_next }
    }

    pub fn keep_custom(&mut self, token: Token, val: &'static [u8]) -> CodeRange {
        self.last_write_state = token.state;
        self.last_write_start = self.write_next;
        self.code[self.write_next..self.write_next + val.len()].copy_from_slice(val);
        self.write_next += val.len();
        CodeRange { start: self.last_write_start, end: self.write_next }
    }

    pub fn keep_with_wrap(&mut self, token: Token, prefix: &'static [u8], suffix: &'static [u8]) -> CodeRange {
        self.last_write_state = token.state;
        self.last_write_start = self.write_next;
        let total_len = prefix.len() + token.accumulated_len + suffix.len();
        self.code[self.write_next..self.write_next + val.len()].copy_from_slice(val);
        self.write_next += val.len();
        CodeRange { start: self.last_write_start, end: self.write_next }
    }

    // If this is called, the last CodeRange returned by a `keep*` method will be invalid.
    pub fn undo_kept(&mut self) -> () {
        self.last_write_state = State::Unknown;
        self.write_next = self.last_write_start;
    }
}
