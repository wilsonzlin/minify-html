use std::ops::Index;

use phf::Set;

use crate::err::{ErrorType, ProcessingResult};
use crate::pattern::SinglePattern;

macro_rules! chain {
    ($proc:ident $($tail:tt)+) => ({
        chain!(@line $proc, last, $($tail)+);
        last
    });
    // Match `?` operator before a call without `?`.
    (@line $proc:ident, $last:ident, . $method:ident($($arg:expr),*)? $($tail:tt)+) => {
        $proc.$method($($arg),*)?;
        chain!(@line $proc, $last, $($tail)*);
    };
    (@line $proc:ident, $last:ident, . $method:ident($($arg:expr),*) $($tail:tt)+) => {
        $proc.$method($($arg),*);
        chain!(@line $proc, $last, $($tail)*);
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
    code: &'d mut [u8],

    // Match.
    // Need to record start as we might get slice after keeping or skipping.
    match_start: usize,
    // Position in output match has been written to. Useful for long term slices where source would already be overwritten.
    match_dest: usize,
    // Guaranteed amount of characters that exist from `start` at time of creation of this struct.
    match_len: usize,
    // Character matched, if any. Only exists for single-character matches and if matched.
    match_char: Option<u8>,
    match_reason: RequireReason,

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

impl<'d> Index<ProcessorRange> for Processor<'d> {
    type Output = [u8];

    fn index(&self, index: ProcessorRange) -> &Self::Output {
        &self.code[index.start..index.end]
    }
}

impl<'d> Processor<'d> {
    // Constructor.
    pub fn new(code: &mut [u8]) -> Processor {
        Processor { write_next: 0, read_next: 0, code, match_start: 0, match_dest: 0, match_len: 0, match_char: None, match_reason: RequireReason::Custom }
    }

    // INTERNAL APIs.
    // Bounds checking.
    fn _in_bounds(&self, offset: usize) -> bool {
        self.read_next + offset < self.code.len()
    }

    // Reading.
    /// Get the `offset` character from next.
    /// When `offset` is 0, the next character is returned.
    /// Panics. Does not check bounds for performance (e.g. already checked).
    fn _read_offset(&self, offset: usize) -> u8 {
        self.code[self.read_next + offset]
    }
    fn _maybe_read_offset(&self, offset: usize) -> Option<u8> {
        if self._in_bounds(offset) {
            Some(self._read_offset(offset))
        } else {
            None
        }
    }

    // Shifting.
    /// Move next `amount` characters to output.
    /// Panics. Does not check bounds for performance (e.g. already checked).
    fn _shift(&mut self, amount: usize) -> () {
        // Optimisation: Don't shift if already there (but still update offsets).
        if self.read_next != self.write_next {
            self.code.copy_within(self.read_next..self.read_next + amount, self.write_next);
        };
        self.read_next += amount;
        self.write_next += amount;
    }

    // Matching.
    // Set match.
    fn _new_match(&mut self, count: usize, char: Option<u8>, reason: RequireReason) -> () {
        // Don't assert match doesn't exist, as otherwise we would need to clear match on every use
        // which would slow down performance and require mutable methods for querying match.
        self.match_start = self.read_next;
        self.match_len = count;
        self.match_char = char;
        self.match_reason = reason;
    }
    fn _match_one<C: FnOnce(u8) -> bool>(&mut self, cond: C, reason: RequireReason) -> () {
        match self._maybe_read_offset(0).filter(|n| cond(*n)) {
            Some(c) => self._new_match(1, Some(c), reason),
            None => self._new_match(0, None, reason),
        }
    }
    fn _match_greedy<C: Fn(u8) -> bool>(&mut self, cond: C) -> () {
        let mut count = 0usize;
        while self._in_bounds(count) && cond(self._read_offset(count)) {
            count += 1;
        };
        self._new_match(count, None, RequireReason::Custom)
    }
    // Ensure that match is nonempty or return error.
    fn _match_require(&self, custom_reason: Option<&'static str>) -> ProcessingResult<()> {
        if self.match_len > 0 {
            Ok(())
        } else {
            match self.match_reason {
                RequireReason::Custom => Err(ErrorType::NotFound(custom_reason.unwrap())),
                RequireReason::ExpectedNotChar(c) => Err(ErrorType::CharNotFound { need: c, got: self.match_char.unwrap() }),
                RequireReason::ExpectedChar(c) => Err(ErrorType::UnexpectedChar(c)),
                RequireReason::ExpectedMatch(m) => Err(ErrorType::MatchNotFound(m)),
            }
        }
    }

    // PUBLIC APIs.
    // Bounds checking
    pub fn at_end(&self) -> bool {
        !self._in_bounds(0)
    }
    /// Get how many characters have been consumed from source.
    pub fn read_len(&self) -> usize {
        self.read_next
    }
    /// Get how many characters have been written to output.
    pub fn written_len(&self) -> usize {
        self.write_next
    }

    // Consume match APIs.
    // Query match.
    pub fn matched(&self) -> bool {
        self.match_len > 0
    }
    pub fn length(&self) -> usize {
        self.match_len
    }
    pub fn char(&self) -> u8 {
        self.match_char.unwrap()
    }
    pub fn maybe_char(&self) -> Option<u8> {
        self.match_char
    }
    pub fn range(&self) -> ProcessorRange {
        ProcessorRange { start: self.match_start, end: self.match_start + self.match_len }
    }
    pub fn out_range(&self) -> ProcessorRange {
        ProcessorRange { start: self.match_dest, end: self.match_dest + self.match_len }
    }
    pub fn slice(&self) -> &[u8] {
        &self.code[self.match_start..self.match_start + self.match_len]
    }
    pub fn out_slice(&self) -> &[u8] {
        &self.code[self.match_dest..self.match_dest + self.match_len]
    }

    // Assert match.
    pub fn require(&self) -> ProcessingResult<()> {
        self._match_require(None)
    }
    pub fn require_with_reason(&self, reason: &'static str) -> ProcessingResult<()> {
        self._match_require(Some(reason))
    }
    // TODO Document
    pub fn expect(&self) -> () {
        debug_assert!(self.match_len > 0);
    }

    // Take action on match.
    // Note that match_len has already been verified to be valid, so don't need to bounds check again.
    pub fn keep(&mut self) -> () {
        self.match_dest = self.write_next;
        self._shift(self.match_len);
    }
    pub fn discard(&mut self) -> () {
        self.read_next = self.match_start + self.match_len;
    }

    // Single-char matching APIs.
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

    // Sequence matching APIs.
    pub fn match_seq(&mut self, pat: &'static [u8]) -> () {
        // For faster short-circuiting matching, compare char-by-char instead of slices.
        let len = pat.len();
        let mut count = 0;
        if len > 0 && self._in_bounds(len - 1) {
            for i in 0..len {
                if self._read_offset(i) != pat[i] {
                    count = 0;
                    break;
                };
                count += 1;
            };
        };
        self._new_match(count, None, RequireReason::Custom)
    }
    pub fn match_line_terminator(&mut self) -> () {
        self._new_match(match self._maybe_read_offset(0) {
            Some(b'\n') => 1,
            Some(b'\r') => 1 + self._maybe_read_offset(1).filter(|c| *c == b'\n').is_some() as usize,
            _ => 0,
        }, None, RequireReason::Custom)
    }

    // Multi-char matching APIs.
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
    pub fn match_while_not_seq(&mut self, s: &SinglePattern) -> () {
        // TODO Test
        // TODO Document
        let count = match s.match_against(&self.code[self.read_next..]) {
            Some(idx) => idx,
            None => self.code.len() - self.read_next,
        };
        self._new_match(count, None, RequireReason::Custom)
    }

    // Checkpoints.
    pub fn checkpoint(&self) -> Checkpoint {
        Checkpoint {
            read_next: self.read_next,
            write_next: self.write_next,
        }
    }
    /// Restore to previously set checkpoint.
    pub fn restore(&mut self, checkpoint: Checkpoint) -> () {
        self.read_next = checkpoint.read_next;
        self.write_next = checkpoint.write_next;
    }
    /// Write characters skipped from source since checkpoint. Must not have written anything since checkpoint.
    pub fn write_skipped(&mut self, checkpoint: Checkpoint) -> () {
        // Make sure that nothing has been written since checkpoint (which would be lost).
        debug_assert_eq!(self.write_next, checkpoint.write_next);
        // Get src code from checkpoint until last consumed character (inclusive).
        let src_start = checkpoint.read_next;
        let src_end = self.read_next;
        self.code.copy_within(src_start..src_end, checkpoint.write_next);
        self.read_next = src_end;
        self.write_next += src_end - src_start;
    }
    /// Discard characters written since checkpoint but keep source position.
    pub fn erase_written(&mut self, checkpoint: Checkpoint) -> () {
        self.write_next = checkpoint.write_next;
    }
    /// Get amount of source characters consumed since checkpoint.
    pub fn consumed_count(&self, checkpoint: Checkpoint) -> usize {
        self.read_next - checkpoint.read_next
    }
    /// Get amount of output characters written since checkpoint.
    pub fn written_count(&self, checkpoint: Checkpoint) -> usize {
        self.write_next - checkpoint.write_next
    }

    // Looking ahead.
    /// Get the `offset` character from next.
    /// When `offset` is 0, the next character is returned.
    pub fn peek_offset_eof(&self, offset: usize) -> Option<u8> {
        self._maybe_read_offset(offset)
    }
    pub fn peek_offset(&self, offset: usize) -> ProcessingResult<u8> {
        self._maybe_read_offset(offset).ok_or(ErrorType::UnexpectedEnd)
    }
    pub fn peek_eof(&self) -> Option<u8> {
        self._maybe_read_offset(0)
    }
    pub fn peek(&self) -> ProcessingResult<u8> {
        self._maybe_read_offset(0).ok_or(ErrorType::UnexpectedEnd)
    }

    // Consuming source characters.
    /// Skip the next `count` characters (can be zero).
    /// Will result in an error if exceeds bounds.
    pub fn skip_amount(&mut self, count: usize) -> ProcessingResult<()> {
        // Check for zero to prevent underflow as type is usize.
        if count == 0 || self._in_bounds(count - 1) {
            self.read_next += count;
            Ok(())
        } else {
            Err(ErrorType::UnexpectedEnd)
        }
    }
    /// Skip and return the next character.
    /// Will result in an error if exceeds bounds.
    pub fn skip(&mut self) -> ProcessingResult<u8> {
        if !self.at_end() {
            let c = self._read_offset(0);
            self.read_next += 1;
            Ok(c)
        } else {
            Err(ErrorType::UnexpectedEnd)
        }
    }

    // Writing characters directly.
    /// Write `c` to output. Will panic if exceeds bounds.
    pub fn write(&mut self, c: u8) -> () {
        self.code[self.write_next] = c;
        self.write_next += 1;
    }
    /// Write `s` to output. Will panic if exceeds bounds.
    pub fn write_slice(&mut self, s: &[u8]) -> () {
        self.code[self.write_next..self.write_next + s.len()].copy_from_slice(s);
        self.write_next += s.len();
    }
    /// Does not check if `c` is a valid Unicode code point.
    pub fn write_utf8(&mut self, c: u32) -> () {
        // TODO Test.
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

    // Shifting characters.
    pub fn accept(&mut self) -> ProcessingResult<u8> {
        if !self.at_end() {
            let c = self._read_offset(0);
            self._shift(1);
            Ok(c)
        } else {
            Err(ErrorType::UnexpectedEnd)
        }
    }
    pub fn accept_amount(&mut self, count: usize) -> ProcessingResult<()> {
        // Check for zero to prevent underflow as type is usize.
        if count == 0 || self._in_bounds(count - 1) {
            self._shift(count);
            Ok(())
        } else {
            Err(ErrorType::UnexpectedEnd)
        }
    }
}
