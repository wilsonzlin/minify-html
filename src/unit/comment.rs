use lazy_static::lazy_static;
use regex::bytes::Regex;
use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;

lazy_static! {
    static ref COMMENT_END: Regex = Regex::new("-->").unwrap();
}

pub fn process_comment(proc: &mut Processor) -> ProcessingResult<()> {
    proc.m(IsSeq(b"<!--"), Discard).expect();
    proc.m(ThroughPat(&COMMENT_END), Discard).require("comment end")?;
    Ok(())
}
