use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
use aho_corasick::AhoCorasick;
use aho_corasick::AhoCorasickBuilder;
use once_cell::sync::Lazy;

static COMMENT_END: Lazy<AhoCorasick> =
  Lazy::new(|| AhoCorasickBuilder::new().build(["-->"]).unwrap());

#[inline(always)]
pub fn process_comment(proc: &mut Processor) -> ProcessingResult<()> {
  proc.m(IsSeq(b"<!--"), Discard).expect();
  proc
    .m(ThroughSeq(&COMMENT_END), Discard)
    .require("comment end")?;
  Ok(())
}
