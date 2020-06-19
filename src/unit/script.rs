use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
use crate::gen::patterns::SCRIPT_END;

pub fn process_script(proc: &mut Processor) -> ProcessingResult<()> {
    // `process_tag` will require closing tag.
    proc.m(WhileNotPat(SCRIPT_END), Keep);
    Ok(())
}
