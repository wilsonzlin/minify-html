use crate::err::ProcessingResult;
use crate::proc::Processor;
use crate::proc::MatchAction::*;
use crate::proc::MatchCond::*;
use crate::proc::MatchMode::*;

pub fn process_bang(proc: &mut Processor) -> ProcessingResult<()> {
    proc.m(Is, Seq(b"<!"), Keep).expect();
    proc.m(WhileNot, Char(b'>'), Keep);
    proc.m(Is, Char(b'>'), Keep).require("Bang close")?;
    Ok(())
}
