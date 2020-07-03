use lazy_static::lazy_static;
use regex::bytes::Regex;
use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;

static SCRIPT_END_STR: &'static str = "</script";

lazy_static! {
    static ref SCRIPT_END: Regex = Regex::new(SCRIPT_END_STR).unwrap();
}

pub fn process_script(proc: &mut Processor) -> ProcessingResult<()> {
    // `process_tag` will require closing tag.
    proc.m(WhileNotPat(&SCRIPT_END, SCRIPT_END_STR.len()), Keep);
    Ok(())
}
