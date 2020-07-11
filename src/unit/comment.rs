use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;

pub fn process_comment(proc: &mut Processor) -> ProcessingResult<()> {
    proc.m(IsSeq(b"<!--"), Discard).expect();
    loop {
        // Use fast memchr.
        let possible = proc.m(ThroughChar(b'>'), Discard).require("comment end")?;
        if proc[possible].ends_with(b"-->") {
            break;
        };
    };
    Ok(())
}
