use aho_corasick::AhoCorasick;
use aho_corasick::AhoCorasickBuilder;
use aho_corasick::MatchKind;
use memchr::memchr;
use minify_html_common::gen::codepoints::ALPHANUMERIC_OR_EQUALS;
use minify_html_common::gen::entities::EntityType;
use minify_html_common::gen::entities::ENTITY;
use minify_html_common::gen::entities::SHORTER_ENCODED_ENTITIES_DECODED;
use minify_html_common::gen::entities::SHORTER_ENCODED_ENTITIES_ENCODED;
use minify_html_common::pattern::TrieNodeMatch;
use once_cell::sync::Lazy;

static SHORTER_ENCODED_ENTITIES_ENCODED_SEARCHER: Lazy<AhoCorasick> = Lazy::new(|| {
  AhoCorasickBuilder::new()
    .dfa(true)
    .match_kind(MatchKind::LeftmostLongest)
    .build(SHORTER_ENCODED_ENTITIES_DECODED)
});

// Encodes ampersands when necessary, as well as UTF-8 sequences that are shorter encoded.
// Does not handle context-specific escaping e.g. `>`, `'`, `"`.
// Set {@param must_end_with_semicolon} to true to pass validation.
pub fn encode_entities(
  mut code: &[u8],
  in_attr_val: bool,
  must_end_with_semicolon: bool,
) -> Vec<u8> {
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
                && code.get(len).is_some_and(|&c| ALPHANUMERIC_OR_EQUALS[c]) =>
            {
              // A named entity inside an attribute value that doesn't end with semicolon but is followed by an alphanumeric or `=` character is not decoded, so we don't need to encode.
              // https://html.spec.whatwg.org/multipage/parsing.html#named-character-reference-state.
              0
            }
            _ => {
              if must_end_with_semicolon {
                res.extend_from_slice(b"&amp;");
              } else {
                res.extend_from_slice(b"&amp");
              };
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
