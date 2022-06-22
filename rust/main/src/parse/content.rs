use aho_corasick::{AhoCorasick, AhoCorasickBuilder, MatchKind};
use lazy_static::lazy_static;
use memchr::memrchr;

use crate::ast::NodeData;
use crate::common::gen::codepoints::TAG_NAME_CHAR;
use crate::common::spec::tag::ns::Namespace;
use crate::common::spec::tag::omission::{can_omit_as_before, can_omit_as_last_node};
use crate::common::spec::tag::void::VOID_TAGS;
use crate::entity::decode::decode_entities;
use crate::parse::bang::parse_bang;
use crate::parse::comment::parse_comment;
use crate::parse::content::ContentType::*;
use crate::parse::doctype::parse_doctype;
use crate::parse::element::{parse_element, parse_tag, peek_tag_name};
use crate::parse::instruction::parse_instruction;
use crate::parse::Code;

#[derive(Copy, Clone, Eq, PartialEq)]
enum ContentType {
    Bang,
    ClosingTag,
    Comment,
    Doctype,
    IgnoredTag,
    Instruction,
    MalformedLeftChevronSlash,
    OmittedClosingTag,
    OpeningTag,
    Text,
}

fn maybe_ignore_html_head_body(
    code: &mut Code,
    typ: ContentType,
    parent: &[u8],
    name: &[u8],
) -> ContentType {
    match (typ, name, parent) {
        (OpeningTag, b"html", _) => {
            if code.seen_html_open {
                IgnoredTag
            } else {
                code.seen_html_open = true;
                typ
            }
        }
        (OpeningTag, b"head", _) => {
            if code.seen_head_open {
                IgnoredTag
            } else {
                code.seen_head_open = true;
                typ
            }
        }
        (ClosingTag, b"head", _) => {
            if code.seen_head_close {
                IgnoredTag
            } else {
                code.seen_head_close = true;
                typ
            }
        }
        (OmittedClosingTag, _, b"head") => {
            code.seen_head_close = true;
            typ
        }
        (OpeningTag, b"body", _) => {
            if code.seen_body_open {
                IgnoredTag
            } else {
                code.seen_body_open = true;
                typ
            }
        }
        _ => typ,
    }
}

fn build_content_type_matcher() -> (AhoCorasick, Vec<ContentType>) {
    let mut patterns = Vec::<Vec<u8>>::new();
    let mut types = Vec::<ContentType>::new();

    // Only when the character after a `<` is TAG_NAME_CHAR is the `<` is an opening tag.
    // Otherwise, the `<` is interpreted literally as part of text.
    for c in 0u8..128u8 {
        if TAG_NAME_CHAR[c] {
            patterns.push(vec![b'<', c]);
            types.push(ContentType::OpeningTag);
        };
    }

    patterns.push(b"</".to_vec());
    types.push(ContentType::ClosingTag);

    patterns.push(b"<?".to_vec());
    types.push(ContentType::Instruction);

    patterns.push(b"<!doctype".to_vec());
    types.push(ContentType::Doctype);

    patterns.push(b"<!".to_vec());
    types.push(ContentType::Bang);

    patterns.push(b"<!--".to_vec());
    types.push(ContentType::Comment);

    (
        AhoCorasickBuilder::new()
            .ascii_case_insensitive(true)
            .dfa(true)
            .match_kind(MatchKind::LeftmostLongest)
            // Keep in sync with order of CONTENT_TYPE_FROM_PATTERN.
            .build(patterns),
        types,
    )
}

lazy_static! {
    static ref CONTENT_TYPE_MATCHER: (AhoCorasick, Vec<ContentType>) = build_content_type_matcher();
}

pub struct ParsedContent {
    pub children: Vec<NodeData>,
    pub closing_tag_omitted: bool,
}

// Use empty slice for `grandparent` or `parent` if none.
pub fn parse_content(
    code: &mut Code,
    ns: Namespace,
    grandparent: &[u8],
    parent: &[u8],
) -> ParsedContent {
    // We assume the closing tag has been omitted until we see one explicitly before EOF (or it has been omitted as per the spec).
    let mut closing_tag_omitted = true;
    let mut nodes = Vec::<NodeData>::new();
    loop {
        let (text_len, mut typ) = match CONTENT_TYPE_MATCHER.0.find(&code.as_slice()) {
            Some(m) => (m.start(), CONTENT_TYPE_MATCHER.1[m.pattern()]),
            None => (code.rem(), Text),
        };
        // Due to dropped malformed code, it's possible for two or more text nodes to be contiguous. Ensure they always get merged into one.
        // NOTE: Even though bangs/comments/etc. have no effect on layout, they still split text (e.g. `&am<!-- -->p`).
        if text_len > 0 {
            let text = decode_entities(code.slice_and_shift(text_len), false);
            match nodes.last_mut() {
                Some(NodeData::Text { value }) => value.extend_from_slice(&text),
                _ => nodes.push(NodeData::Text { value: text }),
            };
        };
        // Check using Parsing.md tag rules.
        if typ == OpeningTag || typ == ClosingTag {
            let name = peek_tag_name(code);
            if typ == OpeningTag {
                debug_assert!(!name.is_empty());
                if can_omit_as_before(parent, &name) {
                    // The upcoming opening tag implicitly closes the current element e.g. `<tr><td>(current position)<td>`.
                    typ = OmittedClosingTag;
                };
            } else {
                if name.is_empty() {
                    // Malformed code, drop until and including next `>`.
                    typ = MalformedLeftChevronSlash;
                } else if grandparent == name.as_slice()
                    && can_omit_as_last_node(grandparent, parent)
                {
                    // The upcoming closing tag implicitly closes the current element e.g. `<tr><td>(current position)</tr>`.
                    // This DOESN'T handle when grandparent doesn't exist (represented by an empty slice). However, in that case it's irrelevant, as it would mean we would be at EOF, and our parser simply auto-closes everything anyway. (Normally we'd have to determine if `<p>Hello` is an error or allowed.)
                    typ = OmittedClosingTag;
                } else if VOID_TAGS.contains(name.as_slice()) {
                    // Closing tag for void element, drop.
                    typ = IgnoredTag;
                } else if parent.is_empty() || parent != name.as_slice() {
                    // Closing tag mismatch, drop.
                    typ = IgnoredTag;
                };
            };
            typ = maybe_ignore_html_head_body(code, typ, parent, &name);
        };
        match typ {
            Text => break,
            OpeningTag => nodes.push(parse_element(code, ns, parent)),
            ClosingTag => {
                closing_tag_omitted = false;
                break;
            }
            Instruction => nodes.push(parse_instruction(code)),
            Bang => nodes.push(parse_bang(code)),
            Comment => nodes.push(parse_comment(code)),
            Doctype => nodes.push(parse_doctype(code)),
            MalformedLeftChevronSlash => code.shift(match memrchr(b'>', code.as_slice()) {
                Some(m) => m + 1,
                None => code.rem(),
            }),
            OmittedClosingTag => {
                closing_tag_omitted = true;
                break;
            }
            IgnoredTag => drop(parse_tag(code)),
        };
    }
    ParsedContent {
        children: nodes,
        closing_tag_omitted,
    }
}
