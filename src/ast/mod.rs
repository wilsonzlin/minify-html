use std::collections::HashMap;

pub enum NodeData {
    Comment {
        code: Vec<u8>,
    },
    Bang {
        code: Vec<u8>,
    },
    Element {
        // If the source doesn't have a closing tag, then we can't add one, as otherwise output could be longer than source.
        closing_tag_omitted: bool,
        name: Vec<u8>,
        attributes: HashMap<Vec<u8>, Vec<u8>>,
        children: Vec<NodeData>,
    },
    Instruction {
        code: Vec<u8>,
    },
    // Entities should not be decoded in ScriptOrStyleContent.
    ScriptOrStyleContent {
        code: Vec<u8>,
    },
    Text {
        code: Vec<u8>,
    },
}
