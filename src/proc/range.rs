use crate::err::ProcessingResult;
use crate::ErrorType;
use crate::proc::Processor;

// TODO DOC
#[derive(Copy, Clone)]
pub struct ProcessorRange {
    pub(super) start: usize,
    pub(super) end: usize,
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
