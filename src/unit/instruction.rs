use crate::err::ProcessingResult;
use crate::proc::Processor;

include!(concat!(env!("OUT_DIR"), "/gen_pattern_INSTRUCTION_END.rs"));

pub fn process_instruction(proc: &mut Processor) -> ProcessingResult<()> {
    if cfg!(debug_assertions) {
        chain!(proc.match_seq(b"<?").expect().keep());
    } else {
        proc.accept_amount_expect(2);
    };

    chain!(proc.match_while_not_seq(INSTRUCTION_END).keep());

    chain!(proc.match_seq(b"?>").require_with_reason("end of processing instruction")?.keep());

    Ok(())
}
