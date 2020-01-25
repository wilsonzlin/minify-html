use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchCond::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;

include!(concat!(env!("OUT_DIR"), "/gen_pattern_INSTRUCTION_END.rs"));

pub fn process_instruction(proc: &mut Processor) -> ProcessingResult<()> {
    proc.m(Is, Seq(b"<?"), Keep).expect();
    proc.m(Through, Pat(INSTRUCTION_END), Keep).require("instruction end")?;
    Ok(())
}
