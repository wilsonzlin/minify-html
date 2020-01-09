use crate::err::ProcessingResult;
use crate::proc::Processor;

include!(concat!(env!("OUT_DIR"), "/gen_pattern_STYLE_END.rs"));

pub fn process_style(proc: &mut Processor) -> ProcessingResult<()> {
    // `process_tag` will require closing tag.
    chain!(proc.match_while_not_seq(STYLE_END).keep());
    Ok(())
}
