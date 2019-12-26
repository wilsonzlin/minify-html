use crate::proc::Processor;
use crate::err::ProcessingResult;
use crate::pattern::SinglePattern;

pub fn process_comment(proc: &mut Processor) -> ProcessingResult<()> {
    chain!(proc.match_seq(b"<!--").expect().discard());

    chain!(proc.match_while_not_seq(&SinglePattern::new(b"-->")).discard());

    chain!(proc.match_seq(b"-->").require_with_reason("comment end")?.discard());

    Ok(())
}
