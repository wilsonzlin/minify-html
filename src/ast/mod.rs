use crate::spec::tag::ns::Namespace;
use std::collections::HashMap;

#[derive(Copy, Clone, Eq, PartialEq)]
pub enum ElementClosingTag {
    Omitted,
    Present,
    SelfClosing,
    Void,
}

#[derive(Copy, Clone, Eq, PartialEq)]
pub enum ScriptOrStyleLang {
    CSS,
    Data,
    JS,
}

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
    Element {
        attributes: HashMap<Vec<u8>, Vec<u8>>,
        children: Vec<NodeData>,
        // If the source doesn't have a closing tag, then we can't add one, as otherwise output could be longer than source.
        closing_tag: ElementClosingTag,
        name: Vec<u8>,
        namespace: Namespace,
    },
    Instruction {
        code: Vec<u8>,
        // If the source unexpectedly ended before `?>`, we can't add it, as otherwise output could be longer than source.
        ended: bool,
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
