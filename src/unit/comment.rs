use crate::proc::Processor;
use crate::err::ProcessingResult;

pub fn process_comment(proc: &mut Processor) -> ProcessingResult<()> {
    chain!(proc.match_seq(b"<!--").expect().discard());

    // TODO Cannot use this pattern
    chain!(proc.match_while_not_seq(b"-->").discard());

    chain!(proc.match_seq(b"-->").require_with_reason("comment end")?.discard());

    Ok(())
}
