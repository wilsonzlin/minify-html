use crate::err::ProcessingResult;
use crate::pattern;
use crate::proc::Processor;

pub fn process_comment(proc: &mut Processor) -> ProcessingResult<()> {
    if cfg!(debug_assertions) {
        chain!(proc.match_seq(b"<!--").expect().discard());
    } else {
        proc.skip_amount_expect(4);
    }

    chain!(proc.match_while_not_seq(pattern::COMMENT_END).discard());

    chain!(proc.match_seq(b"-->").require()?.discard());

    Ok(())
}
