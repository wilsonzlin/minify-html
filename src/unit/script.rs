use crate::err::ProcessingResult;
use crate::proc::Processor;
use crate::proc::MatchAction::*;
use crate::proc::MatchCond::*;
use crate::proc::MatchMode::*;

include!(concat!(env!("OUT_DIR"), "/gen_pattern_SCRIPT_END.rs"));

pub fn process_script(proc: &mut Processor) -> ProcessingResult<()> {
    // `process_tag` will require closing tag.
    proc.m(WhileNot, Pat(SCRIPT_END), Keep);
    Ok(())
}
