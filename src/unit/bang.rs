use crate::proc::Processor;
use crate::err::ProcessingResult;

pub fn process_bang(proc: &mut Processor) -> ProcessingResult<()> {
    chain!(proc.match_seq(b"<!").require()?.keep());

    chain!(proc.match_while_not_char(b'>').keep());

    chain!(proc.match_char(b'>').require()?.keep());

    Ok(())
}
