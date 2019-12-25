use crate::proc::Processor;
use crate::err::InternalResult;

pub fn process_bang<'d, 'p>(proc: &'p mut Processor<'d>) -> InternalResult<()> {
    chain!(proc.match_seq(b"<!").require()?.keep());

    chain!(proc.match_while_not_char(b'>').keep());

    chain!(proc.match_char(b'>').require()?.keep());

    Ok(())
}
