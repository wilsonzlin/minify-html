use crate::proc::Processor;
use crate::err::ProcessingResult;
use crate::pattern;

pub fn process_comment(proc: &mut Processor) -> ProcessingResult<()> {
    chain!(proc.match_seq(b"<!--").expect().discard());

    chain!(proc.match_while_not_seq(pattern::COMMENT_END).discard());

    chain!(proc.match_seq(b"-->").require()?.discard());

    Ok(())
}
