use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;

pub fn process_bang(proc: &mut Processor) -> ProcessingResult<()> {
    proc.m(IsSeq(b"<!"), Keep).expect();
    proc.m(WhileNotChar(b'>'), Keep);
    proc.m(IsChar(b'>'), Keep).require("Bang close")?;
    Ok(())
}
