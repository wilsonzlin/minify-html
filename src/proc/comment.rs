use crate::proc::Processor;
use crate::code::Code;
use crate::err::HbRes;

pub fn process_comment<D: Code>(proc: &Processor<D>) -> HbRes<()> {
    proc.match_seq(b"<!--").expect().discard();

    // TODO Cannot use this pattern
    proc.match_while_not_seq(b"-->").discard();

    proc.match_seq(b"-->").require_with_reason("comment end")?.discard();

    Ok(())
}
