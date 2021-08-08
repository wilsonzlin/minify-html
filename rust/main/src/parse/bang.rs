use crate::ast::NodeData;
use crate::parse::Code;
use memchr::memchr;

pub fn parse_bang(code: &mut Code) -> NodeData {
    debug_assert!(code.as_slice().starts_with(b"<!"));
    code.shift(2);
    let (len, matched) = match memchr(b'>', code.as_slice()) {
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
