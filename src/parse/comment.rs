use aho_corasick::AhoCorasick;
use lazy_static::lazy_static;

use crate::ast::NodeData;
use crate::parse::Code;
use crate::Cfg;

lazy_static! {
    static ref COMMENT_END: AhoCorasick = AhoCorasick::new(&["-->"]);
}

pub fn parse_comment(cfg: &Cfg, code: &mut Code) -> NodeData {
    debug_assert!(code.str().starts_with(b"<!--"));
    code.shift(4);
    let (len, matched) = match COMMENT_END.find(code.str()) {
        Some(m) => (m.start(), m.end() - m.start()),
        None => (code.rem(), 0),
    };
    let data = code.copy_and_shift(len);
    // It might be EOF.
    code.shift(matched);
    NodeData::Comment {
        code: data,
        ended: matched > 0,
    }
}
