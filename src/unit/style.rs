use lazy_static::lazy_static;
use regex::bytes::Regex;
use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;

static STYLE_END_STR: &'static str = "</style";

lazy_static! {
    static ref STYLE_END: Regex = Regex::new(STYLE_END_STR).unwrap();
}

pub fn process_style(proc: &mut Processor) -> ProcessingResult<()> {
    // `process_tag` will require closing tag.
    proc.m(WhileNotPat(&STYLE_END, STYLE_END_STR.len()), Keep);
    Ok(())
}
