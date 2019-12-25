use crate::proc::Processor;
use crate::err::InternalResult;
use crate::spec::codepoint::is_control;
use phf::{Set, phf_set};
use crate::unit::attr::value::process_attr_value;

mod value;

static COLLAPSIBLE_AND_TRIMMABLE_ATTRS: Set<&'static [u8]> = phf_set! {
    b"class",
};

#[derive(Clone, Copy, Eq, PartialEq)]
pub enum AttrType {
    // Special value for `process_tag`.
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

pub fn process_attr<'d, 'p>(proc: &'p mut Processor<'d>) -> InternalResult<AttrType> {
    // Expect `process_attr` to be called at an attribute.
    let name = chain!(proc.match_while_pred(is_name_char).expect().keep().slice());

    // TODO DOC Attr must be case sensitive
    let should_collapse_and_trim_value_ws = COLLAPSIBLE_AND_TRIMMABLE_ATTRS.contains(name);
    let has_value = chain!(proc.match_char(b'=').keep().matched());

    if !has_value {
        Ok(AttrType::NoValue)
    } else {
        process_attr_value(proc, should_collapse_and_trim_value_ws)
    }
}
