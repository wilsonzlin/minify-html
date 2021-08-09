use aho_corasick::{AhoCorasick, AhoCorasickBuilder, MatchKind};
use lazy_static::lazy_static;
use memchr::memchr;

use crate::common::gen::codepoints::ALPHANUMERIC_OR_EQUALS;
use crate::common::gen::entities::{
    EntityType, ENTITY, SHORTER_ENCODED_ENTITIES_DECODED, SHORTER_ENCODED_ENTITIES_ENCODED,
};
use crate::common::pattern::TrieNodeMatch;

lazy_static! {
    static ref SHORTER_ENCODED_ENTITIES_ENCODED_SEARCHER: AhoCorasick = AhoCorasickBuilder::new()
        .dfa(true)
        .match_kind(MatchKind::LeftmostLongest)
        .build(SHORTER_ENCODED_ENTITIES_DECODED);
}

// Encodes ampersands when necessary, as well as UTF-8 sequences that are shorter encoded.
// Does not handle context-specific escaping e.g. `>`, `'`, `"`.
pub fn encode_entities(mut code: &[u8], in_attr_val: bool) -> Vec<u8> {
    let mut res = Vec::<u8>::new();
    while !code.is_empty() {
        let (before, matched) = match memchr(b'&', code) {
            None => (code.len(), false),
            Some(n) => (n, true),
        };
        res.extend_from_slice(&code[..before]);
        code = &code[before..];
        if matched {
            let (start, end) = match ENTITY.longest_matching_prefix(code) {
                // Entity is malformed, so we can just ignore it.
                TrieNodeMatch::NotFound { reached } => (0, reached),
                TrieNodeMatch::Found { len, value } => (
                    match value {
                        EntityType::Named(_)
                            if in_attr_val
                                && code[len - 1] != b';'
                                && code
                                    .get(len)
                                    .filter(|&&c| ALPHANUMERIC_OR_EQUALS[c])
                                    .is_some() =>
                        {
                            // A named entity inside an attribute value that doesn't end with semicolon but is followed by an alphanumeric or `=` character is not decoded, so we don't need to encode.
                            // https://html.spec.whatwg.org/multipage/parsing.html#named-character-reference-state.
                            0
                        }
                        _ => {
                            res.extend_from_slice(b"&amp");
                            // Skip the leading ampersand, as it will be replaced by `&amp`.
                            1
                        }
                    },
                    len,
                ),
            };
            res.extend_from_slice(&code[start..end]);
            code = &code[end..];
        };
    }
    SHORTER_ENCODED_ENTITIES_ENCODED_SEARCHER
        .replace_all_bytes(&res, SHORTER_ENCODED_ENTITIES_ENCODED)
}
