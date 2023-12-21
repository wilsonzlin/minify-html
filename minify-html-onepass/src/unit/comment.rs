use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
use aho_corasick::AhoCorasick;
use lazy_static::lazy_static;

lazy_static! {
    static ref COMMENT_END: AhoCorasick = AhoCorasick::new(&["-->"]);
}

#[inline(always)]
pub fn process_comment(proc: &mut Processor) -> ProcessingResult<()> {
    proc.m(IsSeq(b"<!--"), Discard).expect();
    proc.m(ThroughSeq(&COMMENT_END), Discard)
        .require("comment end")?;
    Ok(())
}
