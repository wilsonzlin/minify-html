use crate::common::gen::attrs::ATTRS;
use crate::common::gen::codepoints::{WHATWG_ATTR_NAME_CHAR, WHITESPACE};
use crate::common::spec::tag::ns::Namespace;
use crate::err::ProcessingResult;
use crate::proc::checkpoint::WriteCheckpoint;
use crate::proc::range::ProcessorRange;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
use crate::unit::attr::value::{
    process_attr_value, skip_attr_value, DelimiterType, ProcessedAttrValue,
};

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

pub fn process_attr(
    proc: &mut Processor,
    ns: Namespace,
    element: ProcessorRange,
) -> ProcessingResult<ProcessedAttr> {
    // It's possible to expect attribute name but not be called at an attribute, e.g. due to whitespace between name and
    // value, which causes name to be considered boolean attribute and `=` to be start of new (invalid) attribute name.
    let name = proc
        .m(WhileInLookup(WHATWG_ATTR_NAME_CHAR), Keep)
        .require("attribute name")?;
    proc.make_lowercase(name);
    let attr_cfg = ATTRS.get(ns, &proc[element], &proc[name]);
    let is_boolean = attr_cfg.filter(|attr| attr.boolean).is_some();
    let after_name = WriteCheckpoint::new(proc);

    // TODO Use attr cfg: collapse, trim, case_sensitive.
    let should_collapse_and_trim_value_ws =
        attr_cfg.filter(|attr| attr.collapse && attr.trim).is_some();
    proc.m(WhileInLookup(WHITESPACE), Discard);
    let has_value = proc.m(IsChar(b'='), Keep).nonempty();

    let (typ, value) = if !has_value {
        (AttrType::NoValue, None)
    } else {
        proc.m(WhileInLookup(WHITESPACE), Discard);
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
                ProcessedAttrValue {
                    delimiter: DelimiterType::Unquoted,
                    value,
                } => (AttrType::Unquoted, value),
                ProcessedAttrValue {
                    delimiter: DelimiterType::Double,
                    value,
                }
                | ProcessedAttrValue {
                    delimiter: DelimiterType::Single,
                    value,
                } => (AttrType::Quoted, value),
            }
        }
    };

    Ok(ProcessedAttr { name, typ, value })
}
