use phf::{phf_set, Set};

use crate::err::ProcessingResult;
use crate::proc::{Processor, ProcessorRange};
use crate::spec::codepoint::{is_control, is_whitespace};
use crate::unit::attr::value::{DelimiterType, process_attr_value, ProcessedAttrValue, skip_attr_value};

mod value;

include!(concat!(env!("OUT_DIR"), "/gen_boolean_attrs.rs"));

static COLLAPSIBLE_AND_TRIMMABLE_ATTRS: Set<&'static [u8]> = phf_set! {
    b"class",
};

#[derive(Clone, Copy, Eq, PartialEq)]
pub enum AttrType {
    Quoted,
    Unquoted,
    NoValue,
}

pub struct ProcessedAttr {
    pub name: ProcessorRange,
    pub typ: AttrType,
    pub value: Option<ProcessorRange>,
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

pub fn process_attr(proc: &mut Processor, element: ProcessorRange) -> ProcessingResult<ProcessedAttr> {
    // It's possible to expect attribute name but not be called at an attribute, e.g. due to whitespace between name and
    // value, which causes name to be considered boolean attribute and `=` to be start of new (invalid) attribute name.
    let name = chain!(proc.match_while_pred(is_name_char).require_with_reason("attribute name")?.keep().out_range());
    let is_boolean = BOOLEAN_ATTRS.contains(&proc[element], &proc[name]);
    let after_name = proc.checkpoint();

    let should_collapse_and_trim_value_ws = COLLAPSIBLE_AND_TRIMMABLE_ATTRS.contains(&proc[name]);
    chain!(proc.match_while_pred(is_whitespace).discard());
    let has_value = chain!(proc.match_char(b'=').keep().matched());

    let (typ, value) = if !has_value {
        (AttrType::NoValue, None)
    } else {
        chain!(proc.match_while_pred(is_whitespace).discard());
        if is_boolean {
            skip_attr_value(proc)?;
            (AttrType::NoValue, None)
        } else {
            match process_attr_value(proc, should_collapse_and_trim_value_ws)? {
                ProcessedAttrValue { value: None, .. } => {
                    // Value is empty, which is equivalent to no value, so discard `=`.
                    debug_assert_eq!(proc.written_count(after_name), 1);
                    proc.erase_written(after_name);
                    (AttrType::NoValue, None)
                }
                ProcessedAttrValue { delimiter: DelimiterType::Unquoted, value } => (AttrType::Unquoted, value),
                ProcessedAttrValue { delimiter: DelimiterType::Double, value } | ProcessedAttrValue { delimiter: DelimiterType::Single, value } => (AttrType::Quoted, value),
            }
        }
    };

    Ok(ProcessedAttr { name, typ, value })
}
