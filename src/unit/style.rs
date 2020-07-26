use lazy_static::lazy_static;
use aho_corasick::AhoCorasick;
use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;

lazy_static! {
    static ref STYLE_END: AhoCorasick = AhoCorasick::new(&["</style"]);
}

pub fn process_style(proc: &mut Processor) -> ProcessingResult<()> {
    proc.require_not_at_end()?;
    proc.m(WhileNotSeq(&STYLE_END), Keep);
    // `process_tag` will require closing tag.

    Ok(())
}
