use crate::err::ProcessingResult;
use crate::proc::Processor;
use crate::proc::MatchAction::*;
use crate::proc::MatchCond::*;
use crate::proc::MatchMode::*;

include!(concat!(env!("OUT_DIR"), "/gen_pattern_COMMENT_END.rs"));

pub fn process_comment(proc: &mut Processor) -> ProcessingResult<()> {
    proc.m(Is, Seq(b"<!--"), Discard).expect();
    proc.m(WhileNot, Pat(COMMENT_END), Discard);
    proc.m(Is, Seq(b"-->"), Discard).require("comment end")?;
    Ok(())
}
