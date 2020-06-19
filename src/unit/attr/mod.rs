use crate::err::ProcessingResult;
use crate::proc::checkpoint::Checkpoint;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
use crate::proc::range::ProcessorRange;
use crate::spec::codepoint::{is_control, is_whitespace};
use crate::unit::attr::value::{DelimiterType, process_attr_value, ProcessedAttrValue, skip_attr_value};
use crate::gen::attrs::ATTRS;
use crate::spec::tag::ns::Namespace;

mod value;

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

pub fn process_attr(proc: &mut Processor, ns: Namespace, element: ProcessorRange) -> ProcessingResult<ProcessedAttr> {
    // It's possible to expect attribute name but not be called at an attribute, e.g. due to whitespace between name and
    // value, which causes name to be considered boolean attribute and `=` to be start of new (invalid) attribute name.
    let name = proc.m(WhilePred(is_name_char), Keep).require("attribute name")?;
    let attr_cfg = ATTRS.get(ns, &proc[element], &proc[name]);
    let is_boolean = attr_cfg.filter(|attr| attr.boolean).is_some();
    let after_name = Checkpoint::new(proc);

    let should_collapse_and_trim_value_ws = attr_cfg.filter(|attr| attr.collapse_and_trim).is_some();
    proc.m(WhilePred(is_whitespace), Discard);
    let has_value = proc.m(IsChar(b'='), Keep).nonempty();

    let (typ, value) = if !has_value {
        (AttrType::NoValue, None)
    } else {
        proc.m(WhilePred(is_whitespace), Discard);
        if is_boolean {
            skip_attr_value(proc)?;
            // Discard `=`.
            debug_assert_eq!(after_name.written_count(proc), 1);
            after_name.erase_written(proc);
            (AttrType::NoValue, None)
        } else {
            match process_attr_value(proc, should_collapse_and_trim_value_ws)? {
                ProcessedAttrValue { value: None, .. } => {
                    // Value is empty, which is equivalent to no value, so discard `=`.
                    debug_assert_eq!(after_name.written_count(proc), 1);
                    after_name.erase_written(proc);
                    (AttrType::NoValue, None)
                }
                ProcessedAttrValue { delimiter: DelimiterType::Unquoted, value } => (AttrType::Unquoted, value),
                ProcessedAttrValue { delimiter: DelimiterType::Double, value } | ProcessedAttrValue { delimiter: DelimiterType::Single, value } => (AttrType::Quoted, value),
            }
        }
    };

    Ok(ProcessedAttr { name, typ, value })
}
