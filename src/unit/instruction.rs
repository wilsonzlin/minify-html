use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;

pub fn process_instruction(proc: &mut Processor) -> ProcessingResult<()> {
    proc.m(IsSeq(b"<?"), Keep).expect();
    loop {
        // Use fast memchr.
        let possible = proc.m(ThroughChar(b'>'), Keep).require("instruction end")?;
        if proc[possible].ends_with(b"?>") {
            break;
        };
    };
    Ok(())
}
