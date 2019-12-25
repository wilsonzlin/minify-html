use std::ops::Range;

// TODO Inline with proc.
pub struct Code<'d> {
    pub data: &'d mut [u8],
}

impl<'d> Code<'d> {
    pub fn len(&self) -> usize {
        self.data.len()
    }

    pub fn read_char(&self, pos: usize) -> u8 {
        self.data[pos]
    }
    pub fn read_slice(&self, range: Range<usize>) -> &[u8] {
        &self.data[range]
    }

    pub fn copy_within(&mut self, src: Range<usize>, to: usize) {
        self.data.copy_within(src, to);
    }

    pub fn write_char(&mut self, pos: usize, c: u8) -> () {
        self.data[pos] = c;
    }
    pub fn write_slice(&mut self, pos: usize, s: &[u8]) -> () {
        self.data[pos..pos + s.len()].copy_from_slice(s);
    }
}
