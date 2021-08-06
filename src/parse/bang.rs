use crate::ast::NodeData;
use crate::parse::Code;
use crate::Cfg;
use memchr::memchr;

pub fn parse_bang(cfg: &Cfg, code: &mut Code) -> NodeData {
    debug_assert!(code.str().starts_with(b"<!"));
    code.shift(2);
    let (len, matched) = match memchr(b'>', code.str()) {
        Some(m) => (m, 1),
        None => (code.rem(), 0),
    };
    let data = code.copy_and_shift(len);
    // It might be EOF.
    code.shift(matched);
    NodeData::Bang {
        code: data,
        ended: matched > 0,
    }
}
