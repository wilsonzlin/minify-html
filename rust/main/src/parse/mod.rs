use crate::common::gen::codepoints::Lookup;

pub mod bang;
pub mod comment;
pub mod content;
pub mod doctype;
pub mod element;
pub mod instruction;
pub mod script;
pub mod style;
#[cfg(test)]
mod tests;
pub mod textarea;
pub mod title;

pub struct Code<'c> {
    code: &'c [u8],
    next: usize,

    pub seen_html_open: bool,
    pub seen_head_open: bool,
    pub seen_head_close: bool,
    pub seen_body_open: bool,
}

#[derive(Copy, Clone)]
pub struct Checkpoint(usize);

impl<'c> Code<'c> {
    pub fn new(code: &[u8]) -> Code {
        Code {
            code,
            next: 0,
            seen_html_open: false,
            seen_head_open: false,
            seen_head_close: false,
            seen_body_open: false,
        }
    }

    pub fn as_slice(&self) -> &[u8] {
        &self.code[self.next..]
    }

    pub fn take_checkpoint(&self) -> Checkpoint {
        Checkpoint(self.next)
    }

    pub fn restore_checkpoint(&mut self, cp: Checkpoint) {
        self.next = cp.0;
    }

    pub fn at_end(&self) -> bool {
        debug_assert!(self.next <= self.code.len());
        self.next == self.code.len()
    }

    pub fn shift_if_next(&mut self, c: u8) -> bool {
        if self.code.get(self.next).filter(|&&n| n == c).is_some() {
            self.next += 1;
            true
        } else {
            false
        }
    }

    pub fn shift_if_next_seq_case_insensitive(&mut self, seq: &[u8]) -> bool {
        if self
            .code
            .get(self.next..self.next + seq.len())
            .filter(|n| n.eq_ignore_ascii_case(seq))
            .is_some()
        {
            self.next += seq.len();
            true
        } else {
            false
        }
    }

    pub fn shift_if_next_in_lookup(&mut self, lookup: &'static Lookup) -> Option<u8> {
        let c = self.code.get(self.next).filter(|&&n| lookup[n]).copied();
        if c.is_some() {
            self.next += 1;
        };
        c
    }

    pub fn shift_if_next_not_in_lookup(&mut self, lookup: &'static Lookup) -> Option<u8> {
        let c = self.code.get(self.next).filter(|&&n| !lookup[n]).copied();
        if c.is_some() {
            self.next += 1;
        };
        c
    }

    pub fn shift(&mut self, n: usize) {
        self.next += n;
    }

    pub fn slice_and_shift(&mut self, n: usize) -> &[u8] {
        let str = &self.code[self.next..self.next + n];
        self.next += n;
        str
    }

    pub fn copy_and_shift(&mut self, n: usize) -> Vec<u8> {
        self.slice_and_shift(n).to_vec()
    }

    pub fn copy_and_shift_while_in_lookup(&mut self, lookup: &'static Lookup) -> Vec<u8> {
        let mut len = 0;
        loop {
            match self.code.get(self.next + len) {
                Some(&c) if lookup[c] => len += 1,
                _ => break,
            };
        }
        self.copy_and_shift(len)
    }

    pub fn slice_and_shift_while_not_in_lookup(&mut self, lookup: &'static Lookup) -> &[u8] {
        let mut len = 0;
        loop {
            match self.code.get(self.next + len) {
                Some(&c) if !lookup[c] => len += 1,
                _ => break,
            };
        }
        self.slice_and_shift(len)
    }

    // Returns the last character matched.
    pub fn shift_while_in_lookup(&mut self, lookup: &'static Lookup) -> Option<u8> {
        let mut last: Option<u8> = None;
        loop {
            match self.code.get(self.next) {
                Some(&c) if lookup[c] => {
                    self.next += 1;
                    last = Some(c);
                }
                _ => break,
            };
        }
        last
    }

    pub fn rem(&self) -> usize {
        self.code.len() - self.next
    }
}
