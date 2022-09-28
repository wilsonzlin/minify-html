use std::str::from_utf8_unchecked;

use aho_corasick::{AhoCorasickBuilder, MatchKind};
use css_minify::optimizations::{Level, Minifier};
use lazy_static::lazy_static;

use crate::common::gen::attrs::ATTRS;
use crate::common::gen::codepoints::DIGIT;
use crate::common::pattern::Replacer;
use crate::common::spec::script::JAVASCRIPT_MIME_TYPES;
use crate::common::spec::tag::ns::Namespace;
use crate::common::whitespace::{
    collapse_whitespace, left_trim, remove_all_whitespace, right_trim,
};
use crate::entity::encode::encode_entities;
use crate::Cfg;

fn build_double_quoted_replacer() -> Replacer {
    let mut patterns = Vec::<Vec<u8>>::new();
    let mut replacements = Vec::<Vec<u8>>::new();

    // Replace all `"` with `&#34`, unless the quote is followed by a digit or semicolon,
    // in which case add a semicolon to the encoded entity.
    for c in "0123456789;".bytes() {
        patterns.push(vec![b'"', c]);
        replacements.push(vec![b'&', b'#', b'3', b'4', b';', c]);
    }
    patterns.push(b"\"".to_vec());
    replacements.push(b"&#34".to_vec());

    Replacer::new(
        AhoCorasickBuilder::new()
            .dfa(true)
            .match_kind(MatchKind::LeftmostLongest)
            .build(patterns),
        replacements,
    )
}

fn build_single_quoted_replacer() -> Replacer {
    let mut patterns = Vec::<Vec<u8>>::new();
    let mut replacements = Vec::<Vec<u8>>::new();

    // Replace all `'` with `&#39`, unless the quote is followed by a digit or semicolon,
    // in which case add a semicolon to the encoded entity.
    for c in "0123456789;".bytes() {
        patterns.push(vec![b'\'', c]);
        replacements.push(vec![b'&', b'#', b'3', b'9', b';', c]);
    }
    patterns.push(b"'".to_vec());
    replacements.push(b"&#39".to_vec());

    Replacer::new(
        AhoCorasickBuilder::new()
            .dfa(true)
            .match_kind(MatchKind::LeftmostLongest)
            .build(patterns),
        replacements,
    )
}

// TODO Sync with WHITESPACE definition.
static WS: &[(u8, &[u8])] = &[
    (b'\x09', b"&#9"),
    (b'\x0a', b"&#10"),
    (b'\x0c', b"&#12"),
    (b'\x0d', b"&#13"),
    (b'\x20', b"&#32"),
];

fn build_unquoted_replacer() -> Replacer {
    let mut patterns = Vec::<Vec<u8>>::new();
    let mut replacements = Vec::<Vec<u8>>::new();

    // Replace all whitespace with a numeric entity, unless the whitespace is followed by a digit or semicolon,
    // in which case add a semicolon to the encoded entity.
    for c in "0123456789;".bytes() {
        for &(ws, rep) in WS {
            patterns.push(vec![ws, c]);
            replacements.push({
                let mut ent = rep.to_vec();
                ent.push(b';');
                ent.push(c);
                ent
            });
        }
    }
    for &(ws, rep) in WS {
        patterns.push(vec![ws]);
        replacements.push(rep.to_vec());
    }

    // Replace all `>` with `&GT`, unless the chevron is followed by a semicolon,
    // in which case add a semicolon to the encoded entity.
    // Use `&GT` instead of `&gt` as `&gt` has more conflicting entities e.g. `&gtcc;`, `&gtdot;`.
    patterns.push(b">;".to_vec());
    replacements.push(b"&GT;;".to_vec());
    patterns.push(b">".to_vec());
    replacements.push(b"&GT".to_vec());

    Replacer::new(
        AhoCorasickBuilder::new()
            .dfa(true)
            .match_kind(MatchKind::LeftmostLongest)
            .build(patterns),
        replacements,
    )
}

// If spec compliance is required, these characters must also be encoded in an unquoted attr value,
// as well as whitespace, `<`, and `>`.
static WHATWG_UNQUOTED: &[(u8, &[u8])] = &[
    (b'"', b"&#34"),
    (b'\'', b"&#39"),
    (b'=', b"&#61"),
    (b'`', b"&#6"),
];

fn build_whatwg_unquoted_replacer() -> Replacer {
    let mut patterns = Vec::<Vec<u8>>::new();
    let mut replacements = Vec::<Vec<u8>>::new();

    // Replace all whitespace with a numeric entity, unless the whitespace is followed by a digit or semicolon,
    // in which case add a semicolon to the encoded entity.
    for c in "0123456789;".bytes() {
        for &(ws, rep) in WS {
            patterns.push(vec![ws, c]);
            replacements.push({
                let mut ent = rep.to_vec();
                ent.push(b';');
                ent.push(c);
                ent
            });
        }
    }
    for &(ws, rep) in WS {
        patterns.push(vec![ws]);
        replacements.push(rep.to_vec());
    }

    // Replace WHATWG-disallowed characters with a numeric entity, unless they're followed by a digit or semicolon,
    // in which case add a semicolon to the encoded entity.
    for c in "0123456789;".bytes() {
        for &(ws, rep) in WHATWG_UNQUOTED {
            patterns.push(vec![ws, c]);
            replacements.push({
                let mut ent = rep.to_vec();
                ent.push(b';');
                ent.push(c);
                ent
            });
        }
    }
    for &(ws, rep) in WHATWG_UNQUOTED {
        patterns.push(vec![ws]);
        replacements.push(rep.to_vec());
    }

    // Replace all `<` with `&LT`, unless the chevron is followed by a semicolon,
    // in which case add a semicolon to the encoded entity.
    // Use `&GT` instead of `&lt` as `&lt` has more conflicting entities e.g. `&ltcc;`, `&ltdot;`.
    patterns.push(b"<;".to_vec());
    replacements.push(b"&LT;;".to_vec());
    patterns.push(b"<".to_vec());
    replacements.push(b"&LT".to_vec());

    // Replace all `>` with `&GT`, unless the chevron is followed by a semicolon,
    // in which case add a semicolon to the encoded entity.
    // Use `&GT` instead of `&gt` as `&gt` has more conflicting entities e.g. `&gtcc;`, `&gtdot;`.
    patterns.push(b">;".to_vec());
    replacements.push(b"&GT;;".to_vec());
    patterns.push(b">".to_vec());
    replacements.push(b"&GT".to_vec());

    Replacer::new(
        AhoCorasickBuilder::new()
            .dfa(true)
            .match_kind(MatchKind::LeftmostLongest)
            .build(patterns),
        replacements,
    )
}

lazy_static! {
    static ref DOUBLE_QUOTED_REPLACER: Replacer = build_double_quoted_replacer();
    static ref SINGLE_QUOTED_REPLACER: Replacer = build_single_quoted_replacer();
    static ref UNQUOTED_REPLACER: Replacer = build_unquoted_replacer();
    static ref WHATWG_UNQUOTED_REPLACER: Replacer = build_whatwg_unquoted_replacer();
}

pub struct AttrMinifiedValue {
    quoted: bool,
    prefix: &'static [u8],
    data: Vec<u8>,
    start: usize,
    suffix: &'static [u8],
}

impl AttrMinifiedValue {
    pub fn quoted(&self) -> bool {
        self.quoted
    }

    pub fn len(&self) -> usize {
        self.prefix.len() + (self.data.len() - self.start) + self.suffix.len()
    }

    pub fn out(&self, out: &mut Vec<u8>) {
        out.extend_from_slice(self.prefix);
        out.extend_from_slice(&self.data[self.start..]);
        out.extend_from_slice(self.suffix);
    }

    #[cfg(test)]
    pub fn str(&self) -> String {
        let mut out = Vec::with_capacity(self.len());
        self.out(&mut out);
        String::from_utf8(out).unwrap()
    }
}

pub fn encode_using_double_quotes(val: &[u8]) -> AttrMinifiedValue {
    AttrMinifiedValue {
        quoted: true,
        prefix: b"\"",
        data: DOUBLE_QUOTED_REPLACER.replace_all(val),
        start: 0,
        suffix: b"\"",
    }
}

pub fn encode_using_single_quotes(val: &[u8]) -> AttrMinifiedValue {
    AttrMinifiedValue {
        quoted: true,
        prefix: b"'",
        data: SINGLE_QUOTED_REPLACER.replace_all(val),
        start: 0,
        suffix: b"'",
    }
}

pub fn encode_unquoted(val: &[u8], whatwg: bool) -> AttrMinifiedValue {
    if whatwg {
        AttrMinifiedValue {
            quoted: false,
            prefix: b"",
            data: WHATWG_UNQUOTED_REPLACER.replace_all(val),
            start: 0,
            suffix: b"",
        }
    } else {
        let data = UNQUOTED_REPLACER.replace_all(val);
        let prefix: &'static [u8] = match data.get(0) {
            Some(b'"') => match data.get(1) {
                Some(&c2) if DIGIT[c2] || c2 == b';' => b"&#34;",
                _ => b"&#34",
            },
            Some(b'\'') => match data.get(1) {
                Some(&c2) if DIGIT[c2] || c2 == b';' => b"&#39;",
                _ => b"&#39",
            },
            _ => b"",
        };
        let start = if !prefix.is_empty() { 1 } else { 0 };
        AttrMinifiedValue {
            quoted: false,
            prefix,
            data,
            start,
            suffix: b"",
        }
    }
}

pub enum AttrMinified {
    Redundant,
    NoValue,
    Value(AttrMinifiedValue),
}

pub fn minify_attr(
    cfg: &Cfg,
    ns: Namespace,
    tag: &[u8],
    // True if element is <meta> and has an attribute `name` equal to `viewport`.
    is_meta_viewport: bool,
    name: &[u8],
    mut value_raw: Vec<u8>,
) -> AttrMinified {
    let attr_cfg = ATTRS.get(ns, tag, name);

    let should_collapse = attr_cfg.filter(|attr| attr.collapse).is_some();
    let should_trim = attr_cfg.filter(|attr| attr.trim).is_some();
    let should_lowercase = attr_cfg.filter(|attr| attr.case_insensitive).is_some();
    let is_boolean = attr_cfg.filter(|attr| attr.boolean).is_some();
    // An attribute can have both redundant_if_empty and default_value, which means it has two default values: "" and default_value.
    let redundant_if_empty = attr_cfg.filter(|attr| attr.redundant_if_empty).is_some();
    let default_value = attr_cfg.and_then(|attr| attr.default_value);

    if is_meta_viewport {
        remove_all_whitespace(&mut value_raw);
    } else {
        // Trim before checking is_boolean as the entire attribute could be redundant post-minification.
        if should_trim {
            right_trim(&mut value_raw);
            left_trim(&mut value_raw);
        };
        if should_collapse {
            collapse_whitespace(&mut value_raw);
        };
    };

    if name == b"style" && cfg.minify_css {
        let mut value_raw_wrapped = String::with_capacity(value_raw.len() + 3);
        // TODO This isn't safe for invalid input e.g. `a}/*`.
        value_raw_wrapped.push_str("x{");
        value_raw_wrapped.push_str(unsafe { from_utf8_unchecked(&value_raw) });
        value_raw_wrapped.push('}');
        let result = Minifier::default().minify(&value_raw_wrapped, Level::Three);
        // TODO Collect error as warning.
        if let Ok(min) = result {
            let mut value_raw_wrapped_min = min.into_bytes();
            // TODO If input was invalid, wrapper syntax may not exist anymore.
            if value_raw_wrapped_min.starts_with(b"x{") {
                value_raw_wrapped_min.drain(0..2);
            };
            if value_raw_wrapped_min.ends_with(b"}") {
                value_raw_wrapped_min.pop();
            };
            value_raw = value_raw_wrapped_min;
        };
    }

    // Make lowercase before checking against default value or JAVASCRIPT_MIME_TYPES.
    if should_lowercase {
        value_raw.make_ascii_lowercase();
    };

    if (value_raw.is_empty() && redundant_if_empty)
        || default_value.filter(|dv| dv == &value_raw).is_some()
        || (tag == b"script"
            && name == b"type"
            && JAVASCRIPT_MIME_TYPES.contains(value_raw.as_slice()))
    {
        return AttrMinified::Redundant;
    };

    if is_boolean || value_raw.is_empty() {
        return AttrMinified::NoValue;
    };

    let encoded = encode_entities(&value_raw, true);

    // When lengths are equal, prefer double quotes to all and single quotes to unquoted.
    let mut min = encode_using_double_quotes(&encoded);
    let sq = encode_using_single_quotes(&encoded);
    if sq.len() < min.len() {
        min = sq;
    };
    let uq = encode_unquoted(
        &encoded,
        cfg.ensure_spec_compliant_unquoted_attribute_values,
    );
    if uq.len() < min.len() {
        min = uq;
    };
    AttrMinified::Value(min)
}
