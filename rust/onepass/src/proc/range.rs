use crate::err::ProcessingResult;
use crate::proc::Processor;
use crate::ErrorType;

#[derive(Copy, Clone)]
pub struct ProcessorRange {
    pub(super) start: usize,
    pub(super) end: usize,
}

impl ProcessorRange {
    #[inline(always)]
    pub fn len(&self) -> usize {
        self.end - self.start
    }

    #[inline(always)]
    pub fn empty(&self) -> bool {
        self.start >= self.end
    }

    #[inline(always)]
    pub fn nonempty(&self) -> bool {
        !self.empty()
    }

    #[inline(always)]
    pub fn first(&self, proc: &Processor) -> Option<u8> {
        if self.empty() {
            None
        } else {
            Some(proc.code[self.start])
        }
    }

    #[inline(always)]
    pub fn require(&self, reason: &'static str) -> ProcessingResult<Self> {
        if self.empty() {
            Err(ErrorType::NotFound(reason))
        } else {
            Ok(*self)
        }
    }

    #[inline(always)]
    pub fn expect(&self) -> () {
        debug_assert!(self.nonempty());
    }
}
