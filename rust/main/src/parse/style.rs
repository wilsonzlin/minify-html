use aho_corasick::AhoCorasick;
use aho_corasick::AhoCorasickBuilder;
use lazy_static::lazy_static;

use crate::ast::{NodeData, ScriptOrStyleLang};
use crate::parse::content::ParsedContent;
use crate::parse::Code;

lazy_static! {
    static ref END: AhoCorasick = AhoCorasickBuilder::new()
        .ascii_case_insensitive(true)
        .build(&["</style"]);
}

pub fn parse_style_content(code: &mut Code) -> ParsedContent {
    let (len, closing_tag_omitted) = match END.find(code.as_slice()) {
        Some(m) => (m.start(), false),
        None => (code.rem(), true),
    };
    ParsedContent {
        closing_tag_omitted,
        children: vec![NodeData::ScriptOrStyleContent {
            code: code.copy_and_shift(len),
            lang: ScriptOrStyleLang::CSS,
        }],
    }
}
