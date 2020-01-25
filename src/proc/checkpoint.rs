use crate::proc::Processor;
use crate::proc::range::ProcessorRange;

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

    pub fn new(proc: &Processor) -> Checkpoint {
        Checkpoint {
            read_next: proc.read_next,
            write_next: proc.write_next,
        }
    }

    pub fn last_written(&self, proc: &mut Processor) -> Option<u8> {
        if proc.write_next <= self.write_next {
            None
        } else {
            Some(proc.code[proc.write_next - 1])
        }
    }

    /// Write characters skipped from source since self. Must not have written anything since self.
    pub fn write_skipped(&mut self, proc: &mut Processor) -> () {
        // Make sure that nothing has been written since checkpoint (which would be lost).
        debug_assert_eq!(proc.write_next, self.write_next);
        // Get src code from checkpoint until last consumed character (inclusive).
        let src_start = self.read_next;
        let src_end = proc.read_next;
        proc.code.copy_within(src_start..src_end, self.write_next);
        proc.write_next += src_end - src_start;
    }

    /// Discard characters written since checkpoint but keep source position.
    pub fn erase_written(&self, proc: &mut Processor) -> () {
        proc.write_next = self.write_next;
    }

    /// Get consumed characters since checkpoint as range.
    pub fn consumed_range(&self, proc: &mut Processor) -> ProcessorRange {
        ProcessorRange { start: self.read_next, end: proc.read_next }
    }

    /// Get written characters since checkpoint as range.
    pub fn written_range(&self, proc: &mut Processor) -> ProcessorRange {
        ProcessorRange { start: self.write_next, end: proc.write_next }
    }

    /// Get amount of source characters consumed since self.
    pub fn consumed_count(&self, proc: &mut Processor) -> usize {
        proc.read_next - self.read_next
    }

    /// Get amount of output characters written since self.
    pub fn written_count(&self, proc: &mut Processor) -> usize {
        proc.write_next - self.write_next
    }
}
