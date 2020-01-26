use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;

include!(concat!(env!("OUT_DIR"), "/gen_pattern_STYLE_END.rs"));

pub fn process_style(proc: &mut Processor) -> ProcessingResult<()> {
    // `process_tag` will require closing tag.
    proc.m(WhileNotPat(STYLE_END), Keep);
    Ok(())
}
