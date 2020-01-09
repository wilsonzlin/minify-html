use crate::err::{ProcessingResult};
use crate::proc::{Processor};

include!(concat!(env!("OUT_DIR"), "/gen_pattern_SCRIPT_END.rs"));

pub fn process_js_script(proc: &mut Processor) -> ProcessingResult<()> {
    // `process_tag` will require closing tag.
    chain!(proc.match_while_not_seq(SCRIPT_END).keep());
    Ok(())
}
