use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
use aho_corasick::AhoCorasick;
use lazy_static::lazy_static;

lazy_static! {
    static ref INSTRUCTION_END: AhoCorasick = AhoCorasick::new(&["?>"]);
}

#[inline(always)]
pub fn process_instruction(proc: &mut Processor) -> ProcessingResult<()> {
    proc.m(IsSeq(b"<?"), Keep).expect();
    proc.m(ThroughSeq(&INSTRUCTION_END), Keep)
        .require("instruction end")?;
    Ok(())
}
