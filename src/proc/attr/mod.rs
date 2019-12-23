use crate::proc::Processor;
use crate::err::HbRes;
use crate::spec::codepoint::is_control;
use crate::code::Code;
use crate::proc::attr::quoted::{is_attr_quote, process_quoted_val};
use crate::proc::attr::unquoted::process_attr_unquoted_val;

mod quoted;
mod unquoted;

pub enum AttrType {
    // Special value for hb_unit_tag.
    None,

    Quoted,
    Unquoted,
    NoValue,
}

// Characters allowed in an attribute name.
// NOTE: Unicode noncharacters not tested.
// See https://html.spec.whatwg.org/multipage/syntax.html#syntax-attribute-name for spec.
fn is_name_char(c: u8) -> bool {
    match c {
        b' ' | b'"' | b'\'' | b'>' | b'/' | b'=' => false,
        c => !is_control(c),
    }
}

pub fn process_attr<D: Code>(proc: &Processor<D>) -> HbRes<AttrType> {
    let name = proc.match_while_pred(is_name_char).require_with_reason("attribute name")?.keep().slice();

    let should_collapse_and_trim_value_ws = name.eq_ignore_ascii_case(b"class");
    let has_value = proc.match_char(b'=').keep().matched();

    if !has_value {
        Ok(AttrType::NoValue)
    } else {
        if proc.match_pred(is_attr_quote).matched() {
            // Quoted attribute value.
            process_quoted_val(proc, should_collapse_and_trim_value_ws)
        } else {
            // Unquoted attribute value.
            process_attr_unquoted_val(proc)?;
            Ok(AttrType::Unquoted)
        }
    }
}
