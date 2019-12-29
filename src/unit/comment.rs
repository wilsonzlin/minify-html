use crate::err::ProcessingResult;
use crate::pattern;
use crate::proc::Processor;

pub fn process_comment(proc: &mut Processor) -> ProcessingResult<()> {
    chain!(proc.match_seq(b"<!--").expect().discard());

    chain!(proc.match_while_not_seq(pattern::COMMENT_END).discard());

    chain!(proc.match_seq(b"-->").require()?.discard());

    Ok(())
}
