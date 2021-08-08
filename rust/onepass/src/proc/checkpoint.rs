use crate::proc::range::ProcessorRange;
use crate::proc::Processor;

#[derive(Copy, Clone)]
pub struct WriteCheckpoint {
    write_next: usize,
}

impl WriteCheckpoint {
    #[inline(always)]
    pub fn get_written_range_since(&self, amount: usize) -> ProcessorRange {
        ProcessorRange {
            start: self.write_next,
            end: self.write_next + amount,
        }
    }

    #[inline(always)]
    pub fn new(proc: &Processor) -> WriteCheckpoint {
        WriteCheckpoint {
            write_next: proc.write_next,
        }
    }

    #[inline(always)]
    pub fn last_written(&self, proc: &mut Processor) -> Option<u8> {
        if proc.write_next <= self.write_next {
            None
        } else {
            Some(proc.code[proc.write_next - 1])
        }
    }

    /// Discard characters written since checkpoint but keep source position.
    #[inline(always)]
    pub fn erase_written(&self, proc: &mut Processor) -> () {
        proc.write_next = self.write_next;
    }

    /// Get written characters since checkpoint as range.
    #[inline(always)]
    pub fn written_range(&self, proc: &mut Processor) -> ProcessorRange {
        ProcessorRange {
            start: self.write_next,
            end: proc.write_next,
        }
    }

    /// Get amount of output characters written since self.
    #[inline(always)]
    pub fn written_count(&self, proc: &mut Processor) -> usize {
        proc.write_next - self.write_next
    }
}

pub struct ReadCheckpoint {
    read_next: usize,
}

impl ReadCheckpoint {
    #[inline(always)]
    pub fn new(proc: &Processor) -> ReadCheckpoint {
        ReadCheckpoint {
            read_next: proc.read_next,
        }
    }

    #[inline(always)]
    pub fn restore(&self, proc: &mut Processor) -> () {
        proc.read_next = self.read_next;
    }
}
