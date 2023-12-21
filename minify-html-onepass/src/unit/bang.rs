use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;

#[inline(always)]
pub fn process_bang(proc: &mut Processor) -> ProcessingResult<()> {
    proc.m(IsSeq(b"<!"), Keep).expect();
    proc.m(ThroughChar(b'>'), Keep).require("bang close")?;
    Ok(())
}
