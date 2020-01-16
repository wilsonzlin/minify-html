use std::ops::{Index, Range, IndexMut};

use fastrie::{Fastrie, FastrieMatch};

use crate::err::{ErrorType, ProcessingResult};
use crate::pattern::SinglePattern;
use crate::spec::codepoint::{is_digit, is_hex_digit, is_whitespace};
use crate::unit::entity::{ENTITY_REFERENCES, is_valid_entity_reference_name_char};

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
    ExpectedMatch(&'static [u8]),
    ExpectedChar(u8),
}

#[derive(Copy, Clone)]
pub struct Checkpoint {
    read_next: usize,
    write_next: usize,
}

impl Checkpoint {
    pub fn get_written_range_since(&self, amount: usize) -> ProcessorRange {
        ProcessorRange {
            start: self.write_next,
            end: self.write_next + amount,
        }
    }
}

// TODO DOC
#[derive(Copy, Clone)]
pub struct ProcessorRange {
    start: usize,
    end: usize,
}

impl ProcessorRange {
    pub fn len(&self) -> usize {
        self.end - self.start
    }
    pub fn empty(&self) -> bool {
        self.start >= self.end
    }
}

#[derive(Eq, PartialEq)]
enum UnintentionalEntityState {
    Safe,
    Ampersand,
    Named,
    AmpersandHash,
    Dec,
    Hex,
}

pub struct UnintentionalEntityPrevention {
    last_write_next: usize,
    ampersand_pos: usize,
    state: UnintentionalEntityState,
}

// Processing state of a file. Most fields are used internally and set during
// processing. Single use only; create one per processing.
pub struct Processor<'d> {
    code: &'d mut [u8],

    // Index of the next character to read.
    read_next: usize,
    // Index of the next unwritten space.
    write_next: usize,

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
}

impl<'d> Index<ProcessorRange> for Processor<'d> {
    type Output = [u8];

    fn index(&self, index: ProcessorRange) -> &Self::Output {
        &self.code[index.start..index.end]
    }
}

impl<'d> IndexMut<ProcessorRange> for Processor<'d> {
    fn index_mut(&mut self, index: ProcessorRange) -> &mut Self::Output {
        debug_assert!(index.end <= self.write_next);
        &mut self.code[index.start..index.end]
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
    fn _replace(&mut self, range: Range<usize>, data: &[u8]) -> () {
        self.code.copy_within(range.end..self.write_next, range.end + data.len() - (range.end - range.start));
        self.code[range.start..range.start + data.len()].copy_from_slice(data);
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
                RequireReason::ExpectedChar(c) => Err(ErrorType::ExpectedChar(c)),
                RequireReason::ExpectedMatch(m) => Err(ErrorType::MatchNotFound(m)),
            }
        }
    }

    pub fn debug_dump(&self) -> String {
        let mut lines = vec![(1, String::new())];
        let mut line_idx = 0;
        let mut indicator_line_idx_opt: Option<usize> = None;
        let mut line_cols = 0;
        let mut line_no = 1;
        for (i, &c) in self.code.iter().enumerate() {
            if i == self.read_next || i == self.write_next {
                let indicator_line_idx = if indicator_line_idx_opt.is_none() {
                    let indicator_line_idx = lines.len();
                    lines.push((-1, String::new()));
                    indicator_line_idx_opt = Some(indicator_line_idx);
                    indicator_line_idx
                } else if let Some(indicator_line_idx) = indicator_line_idx_opt {
                    indicator_line_idx
                } else {
                    unreachable!();
                };
                // At this point, `line_cols` is how many characters are on this line BEFORE this character.
                while line_cols > 0 && lines[indicator_line_idx].1.len() < line_cols {
                    lines[indicator_line_idx].1.push(' ');
                };
                lines[indicator_line_idx].1.push(if i == self.read_next && i == self.write_next {
                    'B'
                } else if i == self.read_next {
                    'R'
                } else {
                    'W'
                })
            };
            match c {
                b'\n' => {
                    lines[line_idx].1.push_str("⏎");
                    line_no += 1;
                    line_cols = 0;
                    line_idx = lines.len();
                    lines.push((line_no, String::new()));
                    indicator_line_idx_opt = None;
                }
                c => {
                    match c {
                        c if is_whitespace(c) => lines[line_idx].1.push('·'),
                        c if c >= b'!' && c <= b'~' => lines[line_idx].1.push(c as char),
                        _ => lines[line_idx].1.push('�'),
                    };
                    line_cols += 1;
                }
            };
        };
        let max_line_no_width = (line_no as f64).log10().ceil() as usize;
        lines
            .iter()
            .map(|(line_no, line)| if *line_no == -1 {
                format!("{:>indent$}|{}", String::from_utf8(vec![b'>'; max_line_no_width]).unwrap(), line, indent = max_line_no_width)
            } else {
                format!("{:>indent$}|{}", line_no, line, indent = max_line_no_width)
            })
            .collect::<Vec<String>>()
            .join("\n")
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
    pub fn match_pred(&mut self, pred: fn(u8) -> bool) -> () {
        self._match_one(pred, RequireReason::Custom)
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
        self._new_match(count, None, RequireReason::ExpectedMatch(pat))
    }
    pub fn match_trie<V: 'static + Copy>(&mut self, trie: &Fastrie<V>) -> Option<V> {
        match trie.longest_matching_prefix(&self.code[self.read_next..]) {
            None => {
                self._new_match(0, None, RequireReason::Custom);
                None
            }
            Some(FastrieMatch { end, value }) => {
                self._new_match(end + 1, None, RequireReason::Custom);
                Some(*value)
            }
        }
    }

    // Multi-char matching APIs.
    pub fn match_while_char(&mut self, c: u8) -> () {
        self._match_greedy(|n| n == c)
    }
    pub fn match_while_not_char(&mut self, c: u8) -> () {
        self._match_greedy(|n| n != c)
    }
    pub fn match_while_pred(&mut self, pred: fn(u8) -> bool) -> () {
        self._match_greedy(pred)
    }
    pub fn match_while_not_pred(&mut self, pred: fn(u8) -> bool) -> () {
        self._match_greedy(|c| !pred(c))
    }
    pub fn match_while_not_seq(&mut self, s: &SinglePattern) -> () {
        let count = match s.match_against(&self.code[self.read_next..]) {
            Some(idx) => idx,
            None => self.code.len() - self.read_next,
        };
        self._new_match(count, None, RequireReason::Custom)
    }

    pub fn maybe_match_char_then_discard(&mut self, c: u8) -> bool {
        let count = match self._maybe_read_offset(0) {
            Some(n) => n == c,
            None => false,
        };
        self.read_next += count as usize;
        count
    }

    // Checkpoints.
    pub fn checkpoint(&self) -> Checkpoint {
        Checkpoint {
            read_next: self.read_next,
            write_next: self.write_next,
        }
    }
    pub fn last_written(&self, checkpoint: Checkpoint) -> Option<u8> {
        if self.write_next <= checkpoint.write_next {
            None
        } else {
            Some(self.code[self.write_next - 1])
        }
    }
    /// Write characters skipped from source since checkpoint. Must not have written anything since checkpoint.
    pub fn write_skipped(&mut self, checkpoint: Checkpoint) -> () {
        // Make sure that nothing has been written since checkpoint (which would be lost).
        debug_assert_eq!(self.write_next, checkpoint.write_next);
        // Get src code from checkpoint until last consumed character (inclusive).
        let src_start = checkpoint.read_next;
        let src_end = self.read_next;
        self.code.copy_within(src_start..src_end, checkpoint.write_next);
        self.write_next += src_end - src_start;
    }
    /// Discard characters written since checkpoint but keep source position.
    pub fn erase_written(&mut self, checkpoint: Checkpoint) -> () {
        self.write_next = checkpoint.write_next;
    }
    /// Get consumed characters since checkpoint as range.
    pub fn consumed_range(&self, checkpoint: Checkpoint) -> ProcessorRange {
        ProcessorRange { start: checkpoint.read_next, end: self.read_next }
    }
    /// Get written characters since checkpoint as range.
    pub fn written_range(&self, checkpoint: Checkpoint) -> ProcessorRange {
        ProcessorRange { start: checkpoint.write_next, end: self.write_next }
    }
    /// Get amount of source characters consumed since checkpoint.
    pub fn consumed_count(&self, checkpoint: Checkpoint) -> usize {
        self.read_next - checkpoint.read_next
    }
    /// Get amount of output characters written since checkpoint.
    pub fn written_count(&self, checkpoint: Checkpoint) -> usize {
        self.write_next - checkpoint.write_next
    }

    pub fn start_preventing_unintentional_entities(&self) -> UnintentionalEntityPrevention {
        UnintentionalEntityPrevention {
            last_write_next: self.write_next,
            ampersand_pos: 0,
            state: UnintentionalEntityState::Safe,
        }
    }
    fn _handle_end_of_possible_entity(&mut self, uep: &mut UnintentionalEntityPrevention, end_inclusive: usize) -> usize {
        let should_encode_ampersand = match uep.state {
            UnintentionalEntityState::Safe => unreachable!(),
            UnintentionalEntityState::Ampersand => unreachable!(),
            UnintentionalEntityState::Named => {
                match ENTITY_REFERENCES.longest_matching_prefix(&self.code[uep.ampersand_pos + 1..=end_inclusive]) {
                    None => false,
                    Some(_) => true,
                }
            }
            UnintentionalEntityState::AmpersandHash => unreachable!(),
            UnintentionalEntityState::Dec | UnintentionalEntityState::Hex => {
                true
            }
        };
        uep.state = UnintentionalEntityState::Safe;
        let encoded = b"amp";
        if should_encode_ampersand {
            // Insert encoded ampersand.
            self._replace(uep.ampersand_pos + 1..uep.ampersand_pos + 1, encoded);
            self.write_next += encoded.len();
            end_inclusive + encoded.len()
        } else {
            end_inclusive
        }
    }
    pub fn after_write(&mut self, uep: &mut UnintentionalEntityPrevention, is_end: bool) -> () {
        let mut i = uep.last_write_next;
        // Use manual loop as `i` and `self.write_next` could change due to mid-array insertion of entities.
        while i < self.write_next {
            let c = self.code[i];
            match uep.state {
                UnintentionalEntityState::Safe => match c {
                    b'&' => {
                        uep.state = UnintentionalEntityState::Ampersand;
                        uep.ampersand_pos = i;
                    }
                    _ => {}
                }
                UnintentionalEntityState::Ampersand => match c {
                    b'#' => {
                        uep.state = UnintentionalEntityState::AmpersandHash;
                    }
                    c if is_valid_entity_reference_name_char(c) => {
                        uep.state = UnintentionalEntityState::Named;
                    }
                    _ => {
                        uep.state = UnintentionalEntityState::Safe;
                    }
                }
                UnintentionalEntityState::AmpersandHash => match c {
                    b'x' => {
                        uep.state = UnintentionalEntityState::Hex;
                    }
                    c if is_digit(c) => {
                        uep.state = UnintentionalEntityState::Dec;
                        i = self._handle_end_of_possible_entity(uep, i);
                    }
                    _ => {
                        uep.state = UnintentionalEntityState::Safe;
                    }
                }
                UnintentionalEntityState::Named => match c {
                    c if is_valid_entity_reference_name_char(c) => {
                        // TODO Maybe should limit count?
                        // NOTE: Cannot try to match trie as characters are consumed as we need to find longest match.
                    }
                    b';' | _ => {
                        i = self._handle_end_of_possible_entity(uep, i);
                    }
                }
                UnintentionalEntityState::Dec => unreachable!(),
                UnintentionalEntityState::Hex => match c {
                    c if is_hex_digit(c) => {
                        i = self._handle_end_of_possible_entity(uep, i);
                    }
                    _ => {
                        uep.state = UnintentionalEntityState::Safe;
                    }
                }
            };
            i += 1;
        };
        if is_end && uep.state == UnintentionalEntityState::Named {
            self._handle_end_of_possible_entity(uep, self.write_next - 1);
        };
        uep.last_write_next = self.write_next;
    }

    pub fn reserve_output(&mut self, amount: usize) -> () {
        self.write_next += amount;
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
    pub fn peek_slice_offset_eof(&self, offset: usize, count: usize) -> Option<&[u8]> {
        if self._in_bounds(offset + count - 1) {
            Some(&self.code[self.read_next + offset..self.read_next + offset + count])
        } else {
            None
        }
    }
    pub fn peek(&self) -> ProcessingResult<u8> {
        self._maybe_read_offset(0).ok_or(ErrorType::UnexpectedEnd)
    }

    // Consuming source characters.
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
    pub fn skip_amount_expect(&mut self, amount: usize) -> () {
        debug_assert!(!self.at_end(), "skip known characters");
        self.read_next += amount;
    }
    pub fn skip_expect(&mut self) -> () {
        debug_assert!(!self.at_end(), "skip known character");
        self.read_next += 1;
    }

    // Writing characters directly.
    /// Write `c` to output. Will panic if exceeds bounds.
    pub fn write(&mut self, c: u8) -> () {
        self.code[self.write_next] = c;
        self.write_next += 1;
    }
    pub fn write_range(&mut self, s: ProcessorRange) -> ProcessorRange {
        let dest_start = self.write_next;
        let dest_end = dest_start + s.len();
        self.code.copy_within(s.start..s.end, dest_start);
        self.write_next = dest_end;
        ProcessorRange { start: dest_start, end: dest_end }
    }
    /// Write `s` to output. Will panic if exceeds bounds.
    pub fn write_slice(&mut self, s: &[u8]) -> () {
        self.code[self.write_next..self.write_next + s.len()].copy_from_slice(s);
        self.write_next += s.len();
    }
    pub fn write_utf8(&mut self, c: char) -> () {
        let mut encoded = [0u8; 4];
        c.encode_utf8(&mut encoded);
        self.write_slice(&encoded);
    }

    // Shifting characters.
    pub fn accept(&mut self) -> ProcessingResult<u8> {
        if !self.at_end() {
            let c = self._read_offset(0);
            self.code[self.write_next] = c;
            self.read_next += 1;
            self.write_next += 1;
            Ok(c)
        } else {
            Err(ErrorType::UnexpectedEnd)
        }
    }
    pub fn accept_expect(&mut self) -> u8 {
        debug_assert!(!self.at_end());
        let c = self._read_offset(0);
        self.code[self.write_next] = c;
        self.read_next += 1;
        self.write_next += 1;
        c
    }
    pub fn accept_amount_expect(&mut self, count: usize) -> () {
        debug_assert!(self._in_bounds(count - 1));
        self._shift(count);
    }
}
