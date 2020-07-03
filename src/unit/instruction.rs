use lazy_static::lazy_static;
use regex::bytes::Regex;
use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;

lazy_static! {
    static ref INSTRUCTION_END: Regex = Regex::new("\\?>").unwrap();
}

pub fn process_instruction(proc: &mut Processor) -> ProcessingResult<()> {
    proc.m(IsSeq(b"<?"), Keep).expect();
    proc.m(ThroughPat(&INSTRUCTION_END), Keep).require("instruction end")?;
    Ok(())
}
