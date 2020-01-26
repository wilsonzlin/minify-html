use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;

include!(concat!(env!("OUT_DIR"), "/gen_pattern_COMMENT_END.rs"));

pub fn process_comment(proc: &mut Processor) -> ProcessingResult<()> {
    proc.m(IsSeq(b"<!--"), Discard).expect();
    proc.m(ThroughPat(COMMENT_END), Discard).require("comment end")?;
    Ok(())
}
