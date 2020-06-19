use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
use crate::gen::patterns::INSTRUCTION_END;

pub fn process_instruction(proc: &mut Processor) -> ProcessingResult<()> {
    proc.m(IsSeq(b"<?"), Keep).expect();
    proc.m(ThroughPat(INSTRUCTION_END), Keep).require("instruction end")?;
    Ok(())
}
