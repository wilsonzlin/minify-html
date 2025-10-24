use aho_corasick::AhoCorasick;
use aho_corasick::AhoCorasickBuilder;
use once_cell::sync::Lazy;

pub static TAG_TEXTAREA_END: Lazy<AhoCorasick> = Lazy::new(|| {
  AhoCorasickBuilder::new()
    .ascii_case_insensitive(true)
    .build(["</textarea"])
    .unwrap()
});

pub static TAG_TITLE_END: Lazy<AhoCorasick> = Lazy::new(|| {
  AhoCorasickBuilder::new()
    .ascii_case_insensitive(true)
    .build(["</title"])
    .unwrap()
});
