use rustc_hash::FxHashMap;
use std::fmt::{Debug, Formatter};
use std::str::from_utf8;

use crate::common::spec::tag::ns::Namespace;

pub mod c14n;

#[derive(Copy, Clone, Eq, PartialEq, Debug)]
pub enum ElementClosingTag {
    Omitted,
    Present,
    SelfClosing,
    Void,
}

#[derive(Copy, Clone, Eq, PartialEq, Debug)]
pub enum ScriptOrStyleLang {
    CSS,
    Data,
    JS,
    JSModule,
}

pub struct AttrVal {
    // For serialisation only, not used for equality or value.
    pub quote: Option<u8>,
    pub value: Vec<u8>,
}

impl AttrVal {
    pub fn as_slice(&self) -> &[u8] {
        self.value.as_slice()
    }
}

impl Debug for AttrVal {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        f.write_str(from_utf8(&self.value).unwrap())
    }
}

impl PartialEq for AttrVal {
    fn eq(&self, other: &Self) -> bool {
        self.value == other.value
    }
}

impl Eq for AttrVal {}

#[derive(Eq, PartialEq, Debug)]
pub enum RcdataContentType {
  Textarea,
  Title,
}

// Derive Eq for testing.
#[derive(Eq, PartialEq)]
pub enum NodeData {
    Bang {
        code: Vec<u8>,
        // If the source unexpectedly ended before `>`, we can't add it, as otherwise output could be longer than source.
        ended: bool,
    },
    Comment {
        code: Vec<u8>,
        // If the source unexpectedly ended before `-->`, we can't add it, as otherwise output could be longer than source.
        ended: bool,
    },
    Doctype {
        legacy: Vec<u8>,
        // If the source unexpectedly ended before `>`, we can't add it, as otherwise output could be longer than source.
        ended: bool,
    },
    Element {
        attributes: FxHashMap<Vec<u8>, AttrVal>,
        children: Vec<NodeData>,
        // If the source doesn't have a closing tag, then we can't add one, as otherwise output could be longer than source.
        closing_tag: ElementClosingTag,
        name: Vec<u8>,
        namespace: Namespace,
        // WARNING: This should only be set during minification, as minification can alter tree (e.g. remove text nodes completely).
        // If the next text or element sibling is an element, this will be set to its tag name.
        // Otherwise, this will be empty. It should be empty on creation.
        next_sibling_element_name: Vec<u8>,
    },
    Instruction {
        code: Vec<u8>,
        // If the source unexpectedly ended before `?>`, we can't add it, as otherwise output could be longer than source.
        ended: bool,
    },
    // <title> or <textarea> element contents.
    RcdataContent {
        typ: RcdataContentType,
        text: Vec<u8>,
    },
    // Entities should not be decoded in ScriptOrStyleContent.
    ScriptOrStyleContent {
        code: Vec<u8>,
        lang: ScriptOrStyleLang,
    },
    Text {
        value: Vec<u8>,
    },
}

impl Debug for NodeData {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        match self {
            NodeData::Bang { code, ended } => f
                .debug_struct("Bang")
                .field("code", &from_utf8(code).unwrap().to_string())
                .field("ended", ended)
                .finish(),
            NodeData::Comment { code, ended } => f
                .debug_struct("Comment")
                .field("code", &from_utf8(code).unwrap().to_string())
                .field("ended", ended)
                .finish(),
            NodeData::Doctype { legacy, ended } => f
                .debug_struct("Doctype")
                .field("legacy", &from_utf8(legacy).unwrap().to_string())
                .field("ended", ended)
                .finish(),
            NodeData::Element {
                attributes,
                children,
                closing_tag,
                name,
                namespace,
                next_sibling_element_name,
            } => f
                .debug_struct("Element")
                .field("tag", &{
                    let mut out = format!("{:?}:{}", namespace, from_utf8(name).unwrap());
                    for (n, v) in attributes {
                        out.push_str(format!(" {}={:?}", from_utf8(n).unwrap(), v).as_str());
                    }
                    out
                })
                .field("children", children)
                .field("closing_tag", closing_tag)
                .field(
                    "next_sibling_element_name",
                    &from_utf8(next_sibling_element_name).unwrap().to_string(),
                )
                .finish(),
            NodeData::Instruction { code, ended } => f
                .debug_struct("Instruction")
                .field("code", &from_utf8(code).unwrap().to_string())
                .field("ended", ended)
                .finish(),
            NodeData::RcdataContent { typ, text } => f
                .debug_struct("RcdataContent")
                .field("typ", typ)
                .field("text", &from_utf8(text).unwrap().to_string())
                .finish(),
            NodeData::ScriptOrStyleContent { code, lang } => f
                .debug_struct("ScriptOrStyleContent")
                .field("code", &from_utf8(code).unwrap().to_string())
                .field("lang", lang)
                .finish(),
            NodeData::Text { value } => f.write_str(from_utf8(value).unwrap()),
        }
    }
}
