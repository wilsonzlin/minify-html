use phf::phf_map;
use crate::pattern::TrieNode;

include!(concat!(env!("OUT_DIR"), "/gen_entities.rs"));

pub fn is_valid_entity_reference_name_char(c: u8) -> bool {
    c >= b'0' && c <= b'9' || c >= b'A' && c <= b'Z' || c >= b'a' && c <= b'z'
}
