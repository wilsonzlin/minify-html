use std::ops::{Index, IndexMut, Range};

use fastrie::Fastrie;

use crate::err::{ErrorType, ProcessingResult};
use crate::pattern::SinglePattern;
use crate::proc::MatchAction::*;
use crate::proc::MatchCond::*;
use crate::proc::MatchMode::*;
use crate::spec::codepoint::{is_digit, is_hex_digit, is_whitespace};
use crate::unit::entity::{ENTITY_REFERENCES, is_valid_entity_reference_name_char};

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
    pub fn nonempty(&self) -> bool {
        !self.empty()
    }
    pub fn first(&self, proc: &Processor) -> Option<u8> {
        if self.empty() {
            None
        } else {
            Some(proc.code[self.start])
        }
    }
    pub fn require(&self, reason: &'static str) -> ProcessingResult<Self> {
        if self.empty() {
            Err(ErrorType::NotFound(reason))
        } else {
            Ok(*self)
        }
    }
    pub fn expect(&self) -> () {
        debug_assert!(self.nonempty());
    }
}

#[derive(Eq, PartialEq)]
enum UnintentionalEntityState {
    Suspended,
    Ended,
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

impl UnintentionalEntityPrevention {
    pub fn expect_active(&self) -> () {
        debug_assert!(match self.state {
            UnintentionalEntityState::Suspended | UnintentionalEntityState::Ended => false,
            _ => true,
        });
    }
}

pub enum MatchCond {
    Is,
    IsNot,
    While,
    WhileNot,
}

pub enum MatchMode {
    Char(u8),
    Pred(fn(u8) -> bool),
    Seq(&'static [u8]),
    Pat(&'static SinglePattern),
}

pub enum MatchAction {
    Keep,
    Discard,
    MatchOnly,
}

// Processing state of a file. Most fields are used internally and set during
// processing. Single use only; create one per processing.
pub struct Processor<'d> {
    code: &'d mut [u8],
    // Index of the next character to read.
    read_next: usize,
    // Index of the next unwritten space.
    write_next: usize,
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
        Processor { write_next: 0, read_next: 0, code }
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
        self.code.get(self.read_next + offset).map(|c| *c)
    }
    fn _maybe_read_slice_offset(&self, offset: usize, count: usize) -> Option<&[u8]> {
        if self._in_bounds(offset + count - 1) {
            Some(&self.code[self.read_next + offset..self.read_next + offset + count])
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
    fn _one<C: FnOnce(u8) -> bool>(&mut self, cond: C) -> usize {
        self._maybe_read_offset(0).filter(|n| cond(*n)).is_some() as usize
    }
    fn _many<C: Fn(u8) -> bool>(&mut self, cond: C) -> usize {
        let mut count = 0usize;
        while self._maybe_read_offset(count).filter(|c| cond(*c)).is_some() {
            count += 1;
        };
        count
    }

    // Make expectation explicit, even for Maybe.
    pub(crate) fn m(&mut self, cond: MatchCond, mode: MatchMode, action: MatchAction) -> ProcessorRange {
        let count = match (cond, mode) {
            (Is, Char(c)) => self._one(|n| n == c),
            (IsNot, Char(c)) => self._one(|n| n != c),
            (While, Char(c)) => self._many(|n| n == c),
            (WhileNot, Char(c)) => self._many(|n| n != c),

            (Is, Pred(p)) => self._one(|n| p(n)),
            (IsNot, Pred(p)) => self._one(|n| !p(n)),
            (While, Pred(p)) => self._many(|n| p(n)),
            (WhileNot, Pred(p)) => self._many(|n| !p(n)),

            // Sequence matching is slow. If using in a loop, use Pat or Trie instead.
            (Is, Seq(seq)) => self._maybe_read_slice_offset(0, seq.len()).filter(|src| *src == seq).map_or(0, |_| seq.len()),
            (IsNot, Seq(seq)) => self._maybe_read_slice_offset(0, seq.len()).filter(|src| *src != seq).map_or(0, |_| seq.len()),
            (While, Seq(_)) => unimplemented!(),
            (WhileNot, Seq(_)) => unimplemented!(),

            (Is, Pat(_)) => unimplemented!(),
            (IsNot, Pat(_)) => unimplemented!(),
            (While, Pat(_)) => unimplemented!(),
            (WhileNot, Pat(pat)) => pat.match_against(&self.code[self.read_next..]).unwrap_or(self.code.len() - self.read_next),
        };
        // If keeping, match will be available in written range (which is better as source might eventually get overwritten).
        // If discarding, then only option is source range.
        let start = match action {
            Discard | MatchOnly => self.read_next,
            Keep => self.write_next,
        };
        match action {
            Discard => self.read_next += count,
            Keep => self._shift(count),
            MatchOnly => {}
        };

        ProcessorRange { start, end: start + count }
    }

    pub(crate) fn trie<V: 'static + Copy>(&mut self, trie: &Fastrie<V>) -> Option<(ProcessorRange, V)> {
        trie.longest_matching_prefix(&self.code[self.read_next..]).map(|m| (
            ProcessorRange { start: self.read_next, end: self.read_next + m.end + 1 },
            *m.value,
        ))
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
            _ => unreachable!(),
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
    fn _after_write(&mut self, uep: &mut UnintentionalEntityPrevention, is_end: bool) -> () {
        let mut i = uep.last_write_next;
        // Use manual loop as `i` and `self.write_next` could change due to mid-array insertion of entities.
        debug_assert!(i <= self.write_next);
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
                _ => unreachable!(),
            };
            i += 1;
        };
        if is_end && uep.state == UnintentionalEntityState::Named {
            self._handle_end_of_possible_entity(uep, self.write_next - 1);
        };
        uep.last_write_next = self.write_next;
    }
    pub fn update(&mut self, uep: &mut UnintentionalEntityPrevention) -> () {
        self._after_write(uep, false);
    }
    pub fn end(&mut self, uep: &mut UnintentionalEntityPrevention) -> () {
        self._after_write(uep, true);
        uep.state = UnintentionalEntityState::Ended;
    }
    pub fn suspend(&mut self, uep: &mut UnintentionalEntityPrevention) -> () {
        if uep.state != UnintentionalEntityState::Suspended {
            self._after_write(uep, true);
            uep.state = UnintentionalEntityState::Suspended;
        };
    }
    pub fn resume(&self, uep: &mut UnintentionalEntityPrevention) -> () {
        debug_assert!(uep.state == UnintentionalEntityState::Suspended);
        uep.last_write_next = self.write_next;
        uep.state = UnintentionalEntityState::Safe;
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
        self._maybe_read_slice_offset(offset, count)
    }
    pub fn peek(&self) -> ProcessingResult<u8> {
        self._maybe_read_offset(0).ok_or(ErrorType::UnexpectedEnd)
    }

    // Consuming source characters.
    /// Skip and return the next character.
    /// Will result in an error if exceeds bounds.
    pub fn skip(&mut self) -> ProcessingResult<u8> {
        self._maybe_read_offset(0).map(|c| {
            self.read_next += 1;
            c
        }).ok_or(ErrorType::UnexpectedEnd)
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
        self._maybe_read_offset(0).map(|c| {
            self.code[self.write_next] = c;
            self.read_next += 1;
            self.write_next += 1;
            c
        }).ok_or(ErrorType::UnexpectedEnd)
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
