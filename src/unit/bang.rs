use crate::err::ProcessingResult;
use crate::proc::Processor;

pub fn process_bang(proc: &mut Processor) -> ProcessingResult<()> {
    if cfg!(debug_assertions) {
        chain!(proc.match_seq(b"<!").expect().keep());
    } else {
        proc.accept_amount_expect(2);
    };

    chain!(proc.match_while_not_char(b'>').keep());

    chain!(proc.match_char(b'>').require()?.keep());

    Ok(())
}
