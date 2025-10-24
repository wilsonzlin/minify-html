use crate::ast::NodeData;
use crate::parse::Code;
use aho_corasick::AhoCorasick;
use aho_corasick::AhoCorasickBuilder;
use once_cell::sync::Lazy;

static INSTRUCTION_END: Lazy<AhoCorasick> =
  Lazy::new(|| AhoCorasickBuilder::new().build(["?>"]).unwrap());

pub fn parse_instruction(code: &mut Code) -> NodeData {
  debug_assert!(code.as_slice().starts_with(b"<?"));
  code.shift(2);
  let (len, matched) = match INSTRUCTION_END.find(code.as_slice()) {
    Some(m) => (m.start(), m.end() - m.start()),
    None => (code.rem(), 0),
  };
  let data = code.copy_and_shift(len);
  // It might be EOF.
  code.shift(matched);
  NodeData::Instruction {
    code: data,
    ended: matched > 0,
  }
}
