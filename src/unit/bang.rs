use crate::proc::Processor;
use crate::err::HbRes;

pub fn process_bang<'d, 'p>(proc: &'p mut Processor<'d>) -> HbRes<()> {
    cascade_return!(proc.match_seq(b"<!").require()?.keep());

    cascade_return!(proc.match_while_not_char(b'>').keep());

    cascade_return!(proc.match_char(b'>').require()?.keep());

    Ok(())
}
