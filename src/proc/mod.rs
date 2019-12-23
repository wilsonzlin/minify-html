use crate::err::{HbErr, HbRes};
use phf::Set;
use crate::code::Code;

pub mod attr;
pub mod bang;
pub mod comment;
pub mod content;
pub mod entity;
pub mod script;
pub mod style;
pub mod tag;

pub enum RequireReason {
    Custom,
    ExpectedNotChar(u8),
    ExpectedMatch(&'static [u8]),
    ExpectedChar(u8),
}

struct Match<'d, D: Code> {
    data: &'d mut D,
    // Need to record start as we might get slice after keeping or skipping.
    start: usize,
    // Guaranteed amount of characters that exist from `start` at time of creation of this struct.
    count: usize,
    // Character matched, if any. Only exists for single-character matches and if matched.
    char: Option<u8>,
    reason: RequireReason,
}

impl<D: Code> Match<'_, D> {
    // Query
    pub fn matched(&self) -> bool {
        self.count > 0
    }
    pub fn length(&self) -> usize {
        self.count
    }
    pub fn char(&self) -> u8 {
        self.char.unwrap()
    }
    pub fn maybe_char(&self) -> Option<u8> {
        self.char
    }
    pub fn slice(&self) -> &[u8] {
        self.data.get_src_slice(self.start..self.start + self.count)
    }

    // Assert
    fn _require(&self, custom_reason: Option<&'static str>) -> HbRes<&Self> {
        if self.count > 0 {
            Ok(self)
        } else {
            match self.reason {
                RequireReason::Custom => Err(HbErr::ExpectedNotFound(custom_reason.unwrap())),
                RequireReason::ExpectedNotChar(c) => Err(HbErr::ExpectedCharNotFound {
                    expected: c,
                    got: self.char.unwrap(),
                }),
                RequireReason::ExpectedChar(c) => Err(HbErr::UnexpectedCharFound(c)),
                RequireReason::ExpectedMatch(m) => Err(HbErr::ExpectedMatchNotFound(m)),
            }
        }
    }
    pub fn require(&self) -> HbRes<&Self> {
        self._require(None)
    }
    pub fn require_with_reason(&self, reason: &'static str) -> HbRes<&Self> {
        self._require(Some(reason))
    }
    // TODO Document
    pub fn expect(&self) -> &Self {
        // TODO Maybe debug_assert?
        assert!(self.count > 0);
        self
    }

    // Commit.
    // Note that self.count has already been verified to be valid, so don't need to bounds check again.
    pub fn keep(&self) -> &Self {
        self.data.shift(self.count);
        self
    }
    pub fn discard(&self) -> &Self {
        self.data.set_src_pos(self.count);
        self
    }
}

struct Checkpoint<'d, D: Code> {
    data: &'d mut D,
    src_pos: usize,
    out_pos: usize,
}

impl<D: Code> Checkpoint<'_, D> {
    pub fn restore(&self) -> () {
        self.data.set_src_pos(self.src_pos);
        self.data.set_out_pos(self.out_pos);
    }

    /// Write characters skipped from source since checkpoint. Must not have written anything since checkpoint.
    pub fn write_skipped(&self) -> () {
        // Make sure that nothing has been written since checkpoint (which would be lost).
        debug_assert_eq!(self.data.get_out_pos(), self.out_pos);
        // Get src code from checkpoint until last consumed character (inclusive).
        let skipped = self.data.get_src_slice(self.src_pos..self.data.get_src_pos());
        self.data.write_slice(skipped);
    }

    /// Discard characters written since checkpoint but keep source position.
    pub fn erase_written(&self) -> () {
        self.data.set_out_pos(self.out_pos);
    }

    pub fn consumed_count(&self) -> usize {
        self.data.get_src_pos() - self.src_pos
    }

    pub fn written_count(&self) -> usize {
        self.data.get_out_pos() - self.out_pos
    }
}

// Processing state of a file. Most fields are used internally and set during
// processing. Single use only; create one per processing.
pub struct Processor<'data, D: Code> {
    pub data: &'data mut D,
}

fn index_of(s: &'static [u8], c: u8, from: usize) -> Option<usize> {
    for i in from..s.len() {
        if s[i] == c {
            return Some(i);
        };
    };
    None
}

// For fast not-matching, ensure that it's possible to continue directly to next character in string
// when searching for first substring matching pattern in string and only partially matching pattern.
// For example, given string "abcdabc" and pattern "abcde", normal substring searching would match
// "abcd", fail, and then start searching from 'b' at index 1. We want to be able to continue searching
// from 'a' at index 4.
macro_rules! debug_assert_fast_pattern {
    ($x:expr) => {
        debug_assert!($x.len() > 0 && index_of($x, $x[0], 1) == None);
    }
}

// For consistency and improvement of underlying API, only write methods in terms of the underlying API (Code methods). Do not call other Proc methods.
// TODO Return refs for matches.
impl<D: Code> Processor<'_, D> {
    // Helper internal functions for match_* API.
    fn _new_match(&self, count: usize, char: Option<u8>, reason: RequireReason) -> Match<D> {
        Match {
            data: self.data,
            start: self.data.get_src_pos(),
            count,
            char,
            reason,
        }
    }
    fn _match_one<C: FnOnce(u8) -> bool>(&self, cond: C, reason: RequireReason) -> Match<D> {
        let m = self.data.maybe_read(0).filter(|n| cond(*n));
        self._new_match(m.is_some() as usize, m, reason)
    }
    fn _match_greedy<C: FnOnce(u8) -> bool>(&self, cond: C) -> Match<D> {
        let mut count = 0usize;
        while self.data.in_bounds(count) && cond(self.data.read(count)) {
            count += 1;
        };
        self._new_match(count, None, RequireReason::Custom)
    }

    // Single-char matching API.
    pub fn match_char(&self, c: u8) -> Match<D> {
        self._match_one(|n| n == c, RequireReason::ExpectedChar(c))
    }
    pub fn match_not_char(&self, c: u8) -> Match<D> {
        self._match_one(|n| n != c, RequireReason::ExpectedNotChar(c))
    }
    pub fn match_member(&self, set: Set<u8>) -> Match<D> {
        self._match_one(|n| set.contains(&n), RequireReason::Custom)
    }
    pub fn match_not_member(&self, set: Set<u8>) -> Match<D> {
        self._match_one(|n| !set.contains(&n), RequireReason::Custom)
    }
    pub fn match_pred(&self, pred: fn(u8) -> bool) -> Match<D> {
        self._match_one(|n| pred(n), RequireReason::Custom)
    }
    pub fn match_not_pred(&self, pred: fn(u8) -> bool) -> Match<D> {
        self._match_one(|n| !pred(n), RequireReason::Custom)
    }

    // Match a sequence of characters.
    pub fn match_seq(&self, pat: &'static [u8]) -> Match<D> {
        debug_assert_fast_pattern!(pat);
        // For faster short-circuiting matching, compare char-by-char instead of slices.
        let len = pat.len();
        let mut count = 0;
        if len > 0 && self.data.in_bounds(len - 1) {
            for i in 0..len {
                if self.data.read(i) != pat[i] {
                    count = 0;
                    break;
                };
                count += 1;
            };
        };
        self._new_match(count, None, RequireReason::Custom)
    }
    pub fn match_line_terminator(&self) -> Match<D> {
        self._new_match(match self.data.maybe_read(0) {
            Some(b'\n') => 1,
            Some(b'\r') => 1 + self.data.maybe_read(1).filter(|c| *c == b'\n').is_some() as usize,
            _ => 0,
        }, None, RequireReason::Custom)
    }

    // Multi-char matching API.
    pub fn match_while_char(&self, c: u8) -> Match<D> {
        self._match_greedy(|n| n == c)
    }
    pub fn match_while_not_char(&self, c: u8) -> Match<D> {
        self._match_greedy(|n| n != c)
    }
    pub fn match_while_member(&self, set: Set<u8>) -> Match<D> {
        self._match_greedy(|n| set.contains(&n))
    }
    pub fn match_while_not_member(&self, set: Set<u8>) -> Match<D> {
        self._match_greedy(|n| !set.contains(&n))
    }
    pub fn match_while_pred(&self, pred: fn(u8) -> bool) -> Match<D> {
        self._match_greedy(pred)
    }
    pub fn match_while_not_seq(&self, s: &'static [u8]) -> Match<D> {
        debug_assert_fast_pattern!(s);
        // TODO Test
        // TODO Document
        let mut count = 0usize;
        let mut srcpos = 0usize;
        // Next character in pattern to match.
        // For example, if `patpos` is 2, we've matched 2 characters so far and need to match character at index 2 in pattern with character `srcpos` in code.
        let mut patpos = 0usize;
        while self.data.in_bounds(srcpos) {
            if self.data.read(srcpos) == s[patpos] {
                if patpos == s.len() - 1 {
                    // Matched last character in pattern i.e. whole pattern.
                    break;
                } else {
                    srcpos += 1;
                    patpos += 1;
                }
            } else {
                count += patpos;
                if patpos == 0 {
                    count += 1;
                    srcpos += 1;
                } else {
                    patpos = 0;
                };
            };
        };
        self._new_match(count, None, RequireReason::Custom)
    }

    pub fn checkpoint(&self) -> Checkpoint<D> {
        Checkpoint {
            data: self.data,
            src_pos: self.data.get_src_pos(),
            out_pos: self.data.get_out_pos(),
        }
    }

    /// Get the `offset` character from next.
    /// When `offset` is 0, the next character is returned.
    pub fn peek_offset_eof(&self, offset: usize) -> Option<u8> {
        self.data.maybe_read(offset)
    }
    pub fn peek_offset(&self, offset: usize) -> HbRes<u8> {
        self.data.maybe_read(offset).ok_or(HbErr::UnexpectedEnd)
    }
    pub fn peek_eof(&self) -> Option<u8> {
        self.data.maybe_read(0)
    }
    pub fn peek(&self) -> HbRes<u8> {
        self.data.maybe_read(0).ok_or(HbErr::UnexpectedEnd)
    }

    /// Skip the next `count` characters (can be zero).
    /// Will result in an error if exceeds bounds.
    pub fn skip_amount(&self, count: usize) -> HbRes<()> {
        // Check for zero to prevent underflow as type is usize.
        if count == 0 || self.data.in_bounds(count - 1) {
            self.data.consume(count);
            Ok(())
        } else {
            Err(HbErr::UnexpectedEnd)
        }
    }
    /// Skip and return the next character.
    /// Will result in an error if exceeds bounds.
    pub fn skip(&self) -> HbRes<u8> {
        if !self.data.at_end() {
            let c = self.data.read(0);
            self.data.consume(1);
            Ok(c)
        } else {
            Err(HbErr::UnexpectedEnd)
        }
    }

    /// Write `c` to output. Will panic if exceeds bounds.
    pub fn write(&self, c: u8) -> () {
        self.data.write(c)
    }
    /// Write `s` to output. Will panic if exceeds bounds.
    pub fn write_slice(&self, s: &[u8]) -> () {
        self.data.write_slice(s)
    }
    /// Does not check if `c` is a valid Unicode code point.
    pub fn write_utf8(&self, c: u32) -> () {
        // Don't use char::encode_utf8 as it requires a valid code point,
        // and requires passing a [u8, 4] which might be heap-allocated.
        if c <= 0x7F {
            // Plain ASCII.
            self.data.write(c as u8);
        } else if c <= 0x07FF {
            // 2-byte UTF-8.
            self.data.write((((c >> 6) & 0x1F) | 0xC0) as u8);
            self.data.write((((c >> 0) & 0x3F) | 0x80) as u8);
        } else if c <= 0xFFFF {
            // 3-byte UTF-8.
            self.data.write((((c >> 12) & 0x0F) | 0xE0) as u8);
            self.data.write((((c >> 6) & 0x3F) | 0x80) as u8);
            self.data.write((((c >> 0) & 0x3F) | 0x80) as u8);
        } else if c <= 0x10FFFF {
            // 4-byte UTF-8.
            self.data.write((((c >> 18) & 0x07) | 0xF0) as u8);
            self.data.write((((c >> 12) & 0x3F) | 0x80) as u8);
            self.data.write((((c >> 6) & 0x3F) | 0x80) as u8);
            self.data.write((((c >> 0) & 0x3F) | 0x80) as u8);
        } else {
            unreachable!();
        }
    }

    pub fn accept(&self) -> HbRes<u8> {
        if !self.data.at_end() {
            let c = self.data.read(0);
            self.data.shift(1);
            Ok(c)
        } else {
            Err(HbErr::UnexpectedEnd)
        }
    }
    pub fn accept_amount(&self, count: usize) -> HbRes<()> {
        // Check for zero to prevent underflow as type is usize.
        if count == 0 || self.data.in_bounds(count - 1) {
            self.data.shift(count);
            Ok(())
        } else {
            Err(HbErr::UnexpectedEnd)
        }
    }
}
