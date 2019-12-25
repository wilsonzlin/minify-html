use std::ops::Index;

use phf::Set;

use crate::code::Code;
use crate::err::{HbErr, HbRes};

macro_rules! cascade_return {
    ($proc:ident $($tail:tt)+) => ({
        cascade_return!(@line $proc, last, $($tail)+);
        last
    });
    // Match `?` operator before a call without `?`.
    (@line $proc:ident, $last:ident, . $method:ident($($arg:expr),*)? $($tail:tt)+) => {
        $proc.$method($($arg),*)?;
        cascade_return!(@line $proc, $last, $($tail)*);
    };
    (@line $proc:ident, $last:ident, . $method:ident($($arg:expr),*) $($tail:tt)+) => {
        $proc.$method($($arg),*);
        cascade_return!(@line $proc, $last, $($tail)*);
    };
    (@line $proc:ident, $last:ident, . $method:ident($($arg:expr),*)?) => {
        let $last = $proc.$method($($arg),*)?;
    };
    (@line $proc:ident, $last:ident, . $method:ident($($arg:expr),*)) => {
        let $last = $proc.$method($($arg),*);
    };
}

#[derive(Copy, Clone)]
pub enum RequireReason {
    Custom,
    ExpectedNotChar(u8),
    ExpectedMatch(&'static [u8]),
    ExpectedChar(u8),
}

#[derive(Copy, Clone)]
struct Match {
    // Need to record start as we might get slice after keeping or skipping.
    start: usize,
    // Guaranteed amount of characters that exist from `start` at time of creation of this struct.
    count: usize,
    // Character matched, if any. Only exists for single-character matches and if matched.
    char: Option<u8>,
    reason: RequireReason,
}

#[derive(Copy, Clone)]
pub struct Checkpoint {
    read_next: usize,
    write_next: usize,
}

// TODO DOC
#[derive(Copy, Clone)]
pub struct ProcessorRange {
    start: usize,
    end: usize,
}

// Processing state of a file. Most fields are used internally and set during
// processing. Single use only; create one per processing.
pub struct Processor<'d> {
    code: Code<'d>,
    m: Option<Match>,
    // Index of the next character to read.
    read_next: usize,
    // Index of the next unwritten space.
    write_next: usize,
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

impl<'d> Index<ProcessorRange> for Processor<'d> {
    type Output = [u8];

    fn index(&self, index: ProcessorRange) -> &Self::Output {
        self.code.read_slice(index.start..index.end)
    }
}

// For consistency and improvement of internal API, only write public functions using internal APIs.
// Do not call other public Processor methods.
impl<'d> Processor<'d> {
    // INTERNAL APIs.
    // Checking bounds.
    fn in_bounds(&self, offset: usize) -> bool {
        self.read_next + offset < self.code.len()
    }

    // Reading.
    /// Get the `offset` character from next.
    /// When `offset` is 0, the next character is returned.
    /// Panics. Does not check bounds for performance (e.g. already checked).
    fn read(&self, offset: usize) -> u8 {
        self.code.read_char(self.read_next + offset)
    }
    fn maybe_read(&self, offset: usize) -> Option<u8> {
        if self.in_bounds(offset) {
            Some(self.read(offset))
        } else {
            None
        }
    }

    // Writing.
    /// Move next `amount` characters to output.
    /// Panics. Does not check bounds for performance (e.g. already checked).
    fn shift(&mut self, amount: usize) -> () {
        self.code.copy_within(self.read_next..self.read_next + amount, self.write_next);
        self.read_next += amount;
    }

    // Skipping.
    /// Panics. Does not check bounds for performance (e.g. already checked).
    fn consume(&mut self, amount: usize) -> () {
        self.read_next += amount;
    }

    pub fn new(code: &mut [u8]) -> Processor {
        Processor { write_next: 0, read_next: 0, code: Code { data: code }, m: None }
    }

    pub fn at_end(&self) -> bool {
        !self.in_bounds(0)
    }
    pub fn written_len(&self) -> usize {
        self.write_next
    }

    // Use match

    // Query
    pub fn matched(&self) -> bool {
        self.m.unwrap().count > 0
    }
    pub fn length(&self) -> usize {
        self.m.unwrap().count
    }
    pub fn char(&self) -> u8 {
        self.m.unwrap().char.unwrap()
    }
    pub fn maybe_char(&self) -> Option<u8> {
        self.m.unwrap().char
    }
    pub fn range(&self) -> ProcessorRange {
        let m = self.m.unwrap();
        ProcessorRange { start: m.start, end: m.start + m.count }
    }
    pub fn slice(&self) -> &[u8] {
        let m = self.m.unwrap();
        self.code.read_slice(m.start..m.start + m.count)
    }

    // Assert
    fn _require(&self, custom_reason: Option<&'static str>) -> HbRes<()> {
        let m = self.m.unwrap();
        if m.count > 0 {
            Ok(())
        } else {
            match m.reason {
                RequireReason::Custom => Err(HbErr::ExpectedNotFound(custom_reason.unwrap())),
                RequireReason::ExpectedNotChar(c) => Err(HbErr::ExpectedCharNotFound { expected: c, got: m.char.unwrap() }),
                RequireReason::ExpectedChar(c) => Err(HbErr::UnexpectedCharFound(c)),
                RequireReason::ExpectedMatch(m) => Err(HbErr::ExpectedMatchNotFound(m)),
            }
        }
    }
    pub fn require(&self) -> HbRes<()> {
        self._require(None)
    }
    pub fn require_with_reason(&self, reason: &'static str) -> HbRes<()> {
        self._require(Some(reason))
    }
    // TODO Document
    pub fn expect(&self) -> () {
        // TODO Maybe debug_assert?
        assert!(self.m.unwrap().count > 0);
    }

    // Commit.
    // Note that m.count has already been verified to be valid, so don't need to bounds check again.
    pub fn keep(&mut self) -> () {
        self.shift(self.m.unwrap().count);
    }
    pub fn discard(&mut self) -> () {
        self.read_next = self.m.unwrap().start + self.m.unwrap().count;
    }

    // Helper internal functions for match_* API.
    fn _new_match(&mut self, count: usize, char: Option<u8>, reason: RequireReason) -> () {
        // Don't assert match doesn't exist, as otherwise we would need to clear match on every use
        // which would slow down performance and require mutable methods for querying match.
        let start = self.read_next;
        self.m = Some(Match { start, count, char, reason });
    }
    fn _match_one<C: FnOnce(u8) -> bool>(&mut self, cond: C, reason: RequireReason) -> () {
        match self.maybe_read(0).filter(|n| cond(*n)) {
            Some(c) => self._new_match(1, Some(c), reason),
            None => self._new_match(0, None, reason),
        }
    }
    fn _match_greedy<C: Fn(u8) -> bool>(&mut self, cond: C) -> () {
        let mut count = 0usize;
        while self.in_bounds(count) && cond(self.read(count)) {
            count += 1;
        };
        self._new_match(count, None, RequireReason::Custom)
    }

    // Single-char matching API.
    pub fn match_char(&mut self, c: u8) -> () {
        self._match_one(|n| n == c, RequireReason::ExpectedChar(c))
    }
    pub fn match_not_char(&mut self, c: u8) -> () {
        self._match_one(|n| n != c, RequireReason::ExpectedNotChar(c))
    }
    pub fn match_member(&mut self, set: Set<u8>) -> () {
        self._match_one(|n| set.contains(&n), RequireReason::Custom)
    }
    pub fn match_not_member(&mut self, set: Set<u8>) -> () {
        self._match_one(|n| !set.contains(&n), RequireReason::Custom)
    }
    pub fn match_pred(&mut self, pred: fn(u8) -> bool) -> () {
        self._match_one(|n| pred(n), RequireReason::Custom)
    }
    pub fn match_not_pred(&mut self, pred: fn(u8) -> bool) -> () {
        self._match_one(|n| !pred(n), RequireReason::Custom)
    }

    // Match a sequence of characters.
    pub fn match_seq(&mut self, pat: &'static [u8]) -> () {
        debug_assert_fast_pattern!(pat);
        // For faster short-circuiting matching, compare char-by-char instead of slices.
        let len = pat.len();
        let mut count = 0;
        if len > 0 && self.in_bounds(len - 1) {
            for i in 0..len {
                if self.read(i) != pat[i] {
                    count = 0;
                    break;
                };
                count += 1;
            };
        };
        self._new_match(count, None, RequireReason::Custom)
    }
    pub fn match_line_terminator(&mut self) -> () {
        self._new_match(match self.maybe_read(0) {
            Some(b'\n') => 1,
            Some(b'\r') => 1 + self.maybe_read(1).filter(|c| *c == b'\n').is_some() as usize,
            _ => 0,
        }, None, RequireReason::Custom)
    }

    // Multi-char matching API.
    pub fn match_while_char(&mut self, c: u8) -> () {
        self._match_greedy(|n| n == c)
    }
    pub fn match_while_not_char(&mut self, c: u8) -> () {
        self._match_greedy(|n| n != c)
    }
    pub fn match_while_member(&mut self, set: Set<u8>) -> () {
        self._match_greedy(|n| set.contains(&n))
    }
    pub fn match_while_not_member(&mut self, set: Set<u8>) -> () {
        self._match_greedy(|n| !set.contains(&n))
    }
    pub fn match_while_pred(&mut self, pred: fn(u8) -> bool) -> () {
        self._match_greedy(pred)
    }
    pub fn match_while_not_seq(&mut self, s: &'static [u8]) -> () {
        debug_assert_fast_pattern!(s);
        // TODO Test
        // TODO Document
        let mut count = 0usize;
        let mut srcpos = 0usize;
        // Next character in pattern to match.
        // For example, if `patpos` is 2, we've matched 2 characters so far and need to match character at index 2 in pattern with character `srcpos` in code.
        let mut patpos = 0usize;
        while self.in_bounds(srcpos) {
            if self.read(srcpos) == s[patpos] {
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

    pub fn checkpoint(&self) -> Checkpoint {
        Checkpoint {
            read_next: self.read_next,
            write_next: self.write_next,
        }
    }

    pub fn restore(&mut self, checkpoint: Checkpoint) -> () {
        self.read_next = checkpoint.read_next;
        self.write_next = checkpoint.write_next;
    }

    /// Write characters skipped from source since checkpoint. Must not have written anything since checkpoint.
    pub fn write_skipped(&mut self, checkpoint: Checkpoint) -> () {
        // Make sure that nothing has been written since checkpoint (which would be lost).
        debug_assert_eq!(self.write_next, checkpoint.write_next);
        // Get src code from checkpoint until last consumed character (inclusive).
        self.code.copy_within(checkpoint.read_next..self.read_next, checkpoint.write_next);
    }

    /// Discard characters written since checkpoint but keep source position.
    pub fn erase_written(&mut self, checkpoint: Checkpoint) -> () {
        self.write_next = checkpoint.write_next;
    }

    pub fn consumed_count(&self, checkpoint: Checkpoint) -> usize {
        self.read_next - checkpoint.read_next
    }

    pub fn written_count(&self, checkpoint: Checkpoint) -> usize {
        self.write_next - checkpoint.write_next
    }

    /// Get the `offset` character from next.
    /// When `offset` is 0, the next character is returned.
    pub fn peek_offset_eof(&self, offset: usize) -> Option<u8> {
        self.maybe_read(offset)
    }
    pub fn peek_offset(&self, offset: usize) -> HbRes<u8> {
        self.maybe_read(offset).ok_or(HbErr::UnexpectedEnd)
    }
    pub fn peek_eof(&self) -> Option<u8> {
        self.maybe_read(0)
    }
    pub fn peek(&self) -> HbRes<u8> {
        self.maybe_read(0).ok_or(HbErr::UnexpectedEnd)
    }

    /// Skip the next `count` characters (can be zero).
    /// Will result in an error if exceeds bounds.
    pub fn skip_amount(&mut self, count: usize) -> HbRes<()> {
        // Check for zero to prevent underflow as type is usize.
        if count == 0 || self.in_bounds(count - 1) {
            self.consume(count);
            Ok(())
        } else {
            Err(HbErr::UnexpectedEnd)
        }
    }
    /// Skip and return the next character.
    /// Will result in an error if exceeds bounds.
    pub fn skip(&mut self) -> HbRes<u8> {
        if !self.at_end() {
            let c = self.read(0);
            self.consume(1);
            Ok(c)
        } else {
            Err(HbErr::UnexpectedEnd)
        }
    }

    /// Write `c` to output. Will panic if exceeds bounds.
    pub fn write(&mut self, c: u8) -> () {
        self.code.write_char(self.write_next, c);
    }
    /// Write `s` to output. Will panic if exceeds bounds.
    pub fn write_slice(&mut self, s: &[u8]) -> () {
        self.code.write_slice(self.write_next, s);
    }
    /// Does not check if `c` is a valid Unicode code point.
    pub fn write_utf8(&mut self, c: u32) -> () {
        // Don't use char::encode_utf8 as it requires a valid code point,
        // and requires passing a [u8, 4] which might be heap-allocated.
        if c <= 0x7F {
            // Plain ASCII.
            self.write(c as u8);
        } else if c <= 0x07FF {
            // 2-byte UTF-8.
            self.write((((c >> 6) & 0x1F) | 0xC0) as u8);
            self.write((((c >> 0) & 0x3F) | 0x80) as u8);
        } else if c <= 0xFFFF {
            // 3-byte UTF-8.
            self.write((((c >> 12) & 0x0F) | 0xE0) as u8);
            self.write((((c >> 6) & 0x3F) | 0x80) as u8);
            self.write((((c >> 0) & 0x3F) | 0x80) as u8);
        } else if c <= 0x10FFFF {
            // 4-byte UTF-8.
            self.write((((c >> 18) & 0x07) | 0xF0) as u8);
            self.write((((c >> 12) & 0x3F) | 0x80) as u8);
            self.write((((c >> 6) & 0x3F) | 0x80) as u8);
            self.write((((c >> 0) & 0x3F) | 0x80) as u8);
        } else {
            unreachable!();
        }
    }

    pub fn accept(&mut self) -> HbRes<u8> {
        if !self.at_end() {
            let c = self.read(0);
            self.shift(1);
            Ok(c)
        } else {
            Err(HbErr::UnexpectedEnd)
        }
    }
    pub fn accept_amount(&mut self, count: usize) -> HbRes<()> {
        // Check for zero to prevent underflow as type is usize.
        if count == 0 || self.in_bounds(count - 1) {
            self.shift(count);
            Ok(())
        } else {
            Err(HbErr::UnexpectedEnd)
        }
    }
}
