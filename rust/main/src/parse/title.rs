use aho_corasick::AhoCorasick;
use aho_corasick::AhoCorasickBuilder;
use lazy_static::lazy_static;

use crate::ast::NodeData;
use crate::entity::decode::decode_entities;
use crate::parse::content::ParsedContent;
use crate::parse::Code;

lazy_static! {
    static ref END: AhoCorasick = AhoCorasickBuilder::new()
        .ascii_case_insensitive(true)
        .build(&["</title"]);
}

pub fn parse_title_content(code: &mut Code) -> ParsedContent {
    let (len, closing_tag_omitted) = match END.find(code.as_slice()) {
        Some(m) => (m.start(), false),
        None => (code.rem(), true),
    };
    ParsedContent {
        closing_tag_omitted,
        children: vec![NodeData::Text {
            value: decode_entities(code.slice_and_shift(len), false),
        }],
    }
}
