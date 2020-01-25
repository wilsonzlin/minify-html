use phf::Map;

use crate::err::ProcessingResult;
use crate::proc::{Processor, ProcessorRange};
use crate::proc::MatchAction::*;
use crate::proc::MatchCond::*;
use crate::proc::MatchMode::*;
use crate::spec::codepoint::{is_control, is_whitespace};
use crate::unit::attr::value::{DelimiterType, process_attr_value, ProcessedAttrValue, skip_attr_value};
use crate::unit::tag::Namespace;

mod value;

pub struct AttributeMinification {
    pub boolean: bool,
    pub redundant_if_empty: bool,
    pub collapse_and_trim: bool,
    pub default_value: Option<&'static [u8]>,
}

pub enum AttrMapEntry {
    AllNamespaceElements(AttributeMinification),
    SpecificNamespaceElements(Map<&'static [u8], AttributeMinification>),
}

#[derive(Clone, Copy)]
pub struct ByNamespace {
    html: Option<&'static AttrMapEntry>,
    svg: Option<&'static AttrMapEntry>,
}

impl ByNamespace {
    fn get(&self, ns: Namespace) -> Option<&'static AttrMapEntry> {
        match ns {
            Namespace::Html => self.html,
            Namespace::Svg => self.svg,
        }
    }
}

pub struct AttrMap(Map<&'static [u8], ByNamespace>);

impl AttrMap {
    pub const fn new(map: Map<&'static [u8], ByNamespace>) -> AttrMap {
        AttrMap(map)
    }

    pub fn get(&self, ns: Namespace, tag: &[u8], attr: &[u8]) -> Option<&AttributeMinification> {
        self.0.get(attr).and_then(|namespaces| namespaces.get(ns)).and_then(|entry| match entry {
            AttrMapEntry::AllNamespaceElements(min) => Some(min),
            AttrMapEntry::SpecificNamespaceElements(map) => map.get(tag),
        })
    }
}

include!(concat!(env!("OUT_DIR"), "/gen_attrs.rs"));

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
    let name = proc.m(While, Pred(is_name_char), Keep).require("attribute name")?;
    let attr_cfg = ATTRS.get(ns, &proc[element], &proc[name]);
    let is_boolean = attr_cfg.filter(|attr| attr.boolean).is_some();
    let after_name = proc.checkpoint();

    let should_collapse_and_trim_value_ws = attr_cfg.filter(|attr| attr.collapse_and_trim).is_some();
    proc.m(While, Pred(is_whitespace), Discard);
    let has_value = proc.m(Is, Char(b'='), Keep).nonempty();

    let (typ, value) = if !has_value {
        (AttrType::NoValue, None)
    } else {
        proc.m(While, Pred(is_whitespace), Discard);
        if is_boolean {
            skip_attr_value(proc)?;
            // Discard `=`.
            debug_assert_eq!(proc.written_count(after_name), 1);
            proc.erase_written(after_name);
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
