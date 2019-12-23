use crate::proc::Processor;
use crate::code::Code;
use crate::err::HbRes;

pub fn process_bang<D: Code>(proc: &Processor<D>) -> HbRes<()> {
    proc.match_seq(b"<!").require()?.keep();

    proc.match_while_not_char(b'>').keep();

    proc.match_char(b'>').require()?.keep();

    Ok(())
}
