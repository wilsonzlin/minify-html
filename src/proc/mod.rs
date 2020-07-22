use core::fmt;
use std::fmt::{Debug, Formatter};
use std::ops::{Index, IndexMut};

use crate::err::{Error, ErrorType, ProcessingResult};
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::range::ProcessorRange;
use memchr::memchr;
use crate::gen::codepoints::{WHITESPACE, Lookup};
#[cfg(feature = "js-esbuild")]
use std::sync::{Arc, Mutex};
#[cfg(feature = "js-esbuild")]
use esbuild_rs::TransformResult;
#[cfg(feature = "js-esbuild")]
use crossbeam::sync::WaitGroup;

pub mod checkpoint;
pub mod entity;
pub mod range;

pub enum MatchMode {
    IsChar(u8),
    IsNotChar(u8),
    WhileChar(u8),
    WhileNotChar(u8),
    // Through is like WhileNot followed by Is, but matches zero if Is is zero.
    ThroughChar(u8),

    IsPred(fn(u8) -> bool),
    IsNotPred(fn(u8) -> bool),
    WhilePred(fn(u8) -> bool),
    WhileNotPred(fn(u8) -> bool),

    IsInLookup(&'static Lookup),
    WhileInLookup(&'static Lookup),
    WhileNotInLookup(&'static Lookup),

    IsSeq(&'static [u8]),
}

pub enum MatchAction {
    Keep,
    Discard,
    MatchOnly,
}

#[cfg(feature = "js-esbuild")]
pub struct JsMinSection {
    pub src: ProcessorRange,
    pub result: TransformResult,
}

// Processing state of a file. Single use only; create one per processing.
pub struct Processor<'d> {
    code: &'d mut [u8],
    // Index of the next character to read.
    read_next: usize,
    // Index of the next unwritten space.
    write_next: usize,
    #[cfg(feature = "js-esbuild")]
    script_wg: WaitGroup,
    #[cfg(feature = "js-esbuild")]
    script_results: Arc<Mutex<Vec<JsMinSection>>>,
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
        Processor {
            write_next: 0,
            read_next: 0,
            code,
            #[cfg(feature = "js-esbuild")]
            script_wg: WaitGroup::new(),
            #[cfg(feature = "js-esbuild")]
            script_results: Arc::new(Mutex::new(Vec::new())),
        }
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
        self.code.get(self.read_next + offset..self.read_next + offset + count)
    }

    /// Move next `amount` characters to output.
    /// Panics. Does not check bounds for performance (e.g. already checked).
    #[inline(always)]
    fn _shift(&mut self, amount: usize) -> () {
        // Optimisation: Don't shift if already there (but still update offsets).
        if self.read_next != self.write_next {
            self.code.copy_within(self.read_next..self.read_next + amount, self.write_next);
        };
        self.read_next += amount;
        self.write_next += amount;
    }

    #[inline(always)]
    fn _replace(&mut self, start: usize, end: usize, data: &[u8]) -> usize {
        debug_assert!(start <= end);
        let added = data.len() - (end - start);
        // Do not allow writing over source.
        debug_assert!(self.write_next + added <= self.read_next);
        self.code.copy_within(end..self.write_next, end + added);
        self.code[start..start + data.len()].copy_from_slice(data);
        // Don't need to update read_next as only data before it has changed.
        self.write_next += added;
        added
    }

    #[inline(always)]
    fn _insert(&mut self, at: usize, data: &[u8]) -> usize {
        self._replace(at, at, data)
    }

    // Matching.
    #[inline(always)]
    fn _one<C: FnOnce(u8) -> bool>(&mut self, cond: C) -> usize {
        self._maybe_read_offset(0).filter(|n| cond(*n)).is_some() as usize
    }

    #[inline(always)]
    fn _many<C: Fn(u8) -> bool>(&mut self, cond: C) -> usize {
        let mut count = 0usize;
        while self._maybe_read_offset(count).filter(|c| cond(*c)).is_some() {
            count += 1;
        };
        count
    }

    fn _remaining(&self) -> usize {
        self.code.len() - self.read_next
    }

    #[inline(always)]
    pub fn m(&mut self, mode: MatchMode, action: MatchAction) -> ProcessorRange {
        let count = match mode {
            IsChar(c) => self._one(|n| n == c),
            IsNotChar(c) => self._one(|n| n != c),
            WhileChar(c) => self._many(|n| n == c),
            WhileNotChar(c) => memchr(c, &self.code[self.read_next..]).unwrap_or(self._remaining()),
            ThroughChar(c) => memchr(c, &self.code[self.read_next..]).map_or(0, |p| p + 1),

            IsInLookup(lookup) => self._one(|n| lookup[n]),
            WhileInLookup(lookup) => self._many(|n| lookup[n]),
            WhileNotInLookup(lookup) => self._many(|n| !lookup[n]),

            IsPred(p) => self._one(|n| p(n)),
            IsNotPred(p) => self._one(|n| !p(n)),
            WhilePred(p) => self._many(|n| p(n)),
            WhileNotPred(p) => self._many(|n| !p(n)),

            IsSeq(seq) => self._maybe_read_slice_offset(0, seq.len()).filter(|src| *src == seq).map_or(0, |_| seq.len()),
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

    // PUBLIC APIs.
    // Bounds checking
    pub fn at_end(&self) -> bool {
        !self._in_bounds(0)
    }

    pub fn require_not_at_end(&self) -> ProcessingResult<()> {
        if self.at_end() {
            Err(ErrorType::UnexpectedEnd)
        } else {
            Ok(())
        }
    }

    /// Get how many characters have been consumed from source.
    pub fn read_len(&self) -> usize {
        self.read_next
    }

    pub fn reserve_output(&mut self, amount: usize) -> () {
        self.write_next += amount;
    }

    // Looking ahead.
    /// Get the `offset` character from next.
    /// When `offset` is 0, the next character is returned.
    pub fn peek(&self, offset: usize) -> Option<u8> {
        self._maybe_read_offset(offset)
    }

    pub fn peek_many(&self, offset: usize, count: usize) -> Option<&[u8]> {
        self._maybe_read_slice_offset(offset, count)
    }

    // Looking behind.
    pub fn last(&self, count: usize) -> &[u8] {
        self.code.get(self.write_next - count..self.write_next).unwrap()
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
        self.write_slice(c.encode_utf8(&mut encoded).as_bytes());
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

    #[cfg(feature = "js-esbuild")]
    pub fn new_script_section(&self) -> (WaitGroup, Arc<Mutex<Vec<JsMinSection>>>) {
        (self.script_wg.clone(), self.script_results.clone())
    }

    // Since we consume the Processor, we must provide a full Error with positions.
    #[cfg(not(feature = "js-esbuild"))]
    pub fn finish(self) -> Result<usize, Error> {
        debug_assert!(self.at_end());
        Ok(self.write_next)
    }

    // Since we consume the Processor, we must provide a full Error with positions.
    #[cfg(feature = "js-esbuild")]
    pub fn finish(self) -> Result<usize, Error> {
        debug_assert!(self.at_end());
        self.script_wg.wait();
        let mut results = Arc::try_unwrap(self.script_results)
            .unwrap_or_else(|_| panic!("failed to acquire script results"))
            .into_inner()
            .unwrap();
        results.sort_unstable_by_key(|r| r.src.start);
        // As we write minified JS code for sections from left to right, we will be shifting code
        // towards the left as previous source JS code sections shrink. We need to keep track of
        // the write pointer after previous compaction.
        // If there are no script sections, then we get self.write_next which will be returned.
        let mut write_next = results.get(0).map_or(self.write_next, |r| r.src.start);
        for (i, JsMinSection { result, src }) in results.iter().enumerate() {
            // Resulting minified JS to write.
            let mut js = result.js.trim().as_bytes();
            // If minified result is actually longer than source, then write source instead.
            // NOTE: We still need to write source as previous iterations may have shifted code down.
            if js.len() >= src.len() {
                js = &self.code[src.start..src.end];
            };
            let write_end = write_next + js.len();
            self.code[write_next..write_end].copy_from_slice(js);
            let next_start = results.get(i + 1).map_or(self.write_next, |r| r.src.start);
            self.code.copy_within(src.end..next_start, write_end);
            write_next = write_end + (next_start - src.end);
        };
        Ok(write_next)
    }
}

impl Debug for Processor<'_> {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
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
                        c if WHITESPACE[c] => lines[line_idx].1.push('·'),
                        c if c >= b'!' && c <= b'~' => lines[line_idx].1.push(c as char),
                        _ => lines[line_idx].1.push('�'),
                    };
                    line_cols += 1;
                }
            };
        };
        let max_line_no_width = (line_no as f64).log10().ceil() as usize;
        // Don't use for_each as otherwise we can't return errors.
        for l in lines
            .iter()
            .map(|(line_no, line)| if *line_no == -1 {
                format!("{:>indent$}|{}\n", String::from_utf8(vec![b'>'; max_line_no_width]).unwrap(), line, indent = max_line_no_width)
            } else {
                format!("{:>indent$}|{}\n", line_no, line, indent = max_line_no_width)
            })
        {
            f.write_str(l.as_str())?;
        }
        Ok(())
    }
}
