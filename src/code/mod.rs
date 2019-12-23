use std::ops::Range;

pub trait Code {
    // Unsafe direct memory access.
    // TODO Pos refers to index of next readable.
    unsafe fn get_src_pos(&self) -> usize;
    /// Does NOT check bounds (assumes already checked).
    unsafe fn set_src_pos(&self, pos: usize) -> ();
    unsafe fn get_src_char_at(&self, pos: usize) -> u8;
    /// Get a slice from `start` (inclusive) to `end` (exclusive).
    unsafe fn get_src_slice(&self, range: Range<usize>) -> &[u8];

    // TODO Pos refers to index of next writable.
    unsafe fn get_out_pos(&self) -> usize;
    /// Does NOT check bounds (assumes already checked).
    unsafe fn set_out_pos(&self, pos: usize) -> usize;
    unsafe fn set_out_char_at(&self, pos: usize, c: u8) -> ();
    unsafe fn get_out_mut_slice(&self, range: Range<usize>) -> &mut [u8];
    unsafe fn replace_out_at(&self, pos: usize, s: &[u8]) -> ();

    // Checking bounds.
    fn in_bounds(&self, offset: usize) -> bool;
    fn at_end(&self) -> bool {
        !self.in_bounds(0)
    }

    // Reading.
    /// Get the `offset` character from next.
    /// When `offset` is 0, the next character is returned.
    /// Panics. Does not check bounds for performance (e.g. already checked).
    fn read(&self, offset: usize) -> u8 {
        self.get_src_char_at(self.get_src_pos() + offset)
    }
    fn maybe_read(&self, offset: usize) -> Option<u8> {
        if self.in_bounds(offset) {
            Some(self.read(offset))
        } else {
            None
        }
    }
    /// Get a slice of the next `count` characters from next.
    /// Panics. Does not check bounds for performance (e.g. already checked).
    fn read_slice(&self, count: usize) -> &[u8] {
        self.get_src_slice(self.get_src_pos()..self.get_src_pos() + count)
    }

    // Writing.
    /// Move next `amount` characters to output.
    /// Panics. Does not check bounds for performance (e.g. already checked).
    fn shift(&self, amount: usize) -> ();
    fn write(&self, c: u8) -> ();
    fn write_slice(&self, s: &[u8]) -> ();

    // Skipping.
    /// Panics. Does not check bounds for performance (e.g. already checked).
    fn consume(&self, amount: usize) -> ();
}
