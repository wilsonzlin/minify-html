use crate::proc::Processor;
use crate::err::{HbRes, HbErr};
use crate::spec::codepoint::is_whitespace;
use crate::code::Code;
use crate::proc::entity::process_entity;

// Characters not allowed in an unquoted attribute value.
// See https://html.spec.whatwg.org/multipage/syntax.html#unquoted for spec.
fn is_valid_unquoted_value_char(c: u8) -> bool {
    match c {
        b'"' | b'\'' | b'`' | b'=' | b'<' | b'>' => true,
        c => !is_whitespace(c),
    }
}

// TODO Unquoted could be optimised to quoted if used entities to encode illegal chars.
pub fn process_attr_unquoted_val<D: Code>(proc: &Processor<D>) -> HbRes<()> {
    let mut at_least_one_char = false;

    loop {
        if proc.match_char(b'&').matched() {
            // Process entity.
            // TODO Entity could decode to illegal character.
            process_entity(proc);
        } else if !proc.match_pred(is_valid_unquoted_value_char).keep().matched() {
            break;
        }
        at_least_one_char = true;
    }

    if !at_least_one_char {
        Err(HbErr::ExpectedNotFound("Expected unquoted attribute value"))
    } else {
        Ok(())
    }
}
