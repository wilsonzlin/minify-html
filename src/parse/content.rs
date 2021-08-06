use aho_corasick::{AhoCorasick, AhoCorasickBuilder, MatchKind};
use lazy_static::lazy_static;
use memchr::memrchr;

use crate::ast::NodeData;
use crate::parse::bang::parse_bang;
use crate::parse::comment::parse_comment;
use crate::parse::content::ContentType::*;
use crate::parse::element::{parse_element, parse_tag, peek_tag_name};
use crate::parse::instruction::parse_instruction;
use crate::parse::Code;
use crate::spec::entity::decode::decode_entities;
use crate::spec::tag::ns::Namespace;
use crate::spec::tag::omission::{can_omit_as_before, can_omit_as_last_node};
use crate::spec::tag::void::VOID_TAGS;

#[derive(Copy, Clone, Eq, PartialEq)]
enum ContentType {
    Text,
    OpeningTag,
    ClosingTag,
    Instruction,
    Bang,
    Comment,
    MalformedLeftChevronSlash,
    OmittedClosingTag,
    ClosingTagForVoidElement,
}

lazy_static! {
    static ref CONTENT_TYPE_PATTERN: AhoCorasick = AhoCorasickBuilder::new()
        .dfa(true)
        .match_kind(MatchKind::LeftmostLongest)
        // Keep in sync with order of CONTENT_TYPE_FROM_PATTERN.
        .build(&[
            "<",
            "</",
            "<?",
            "<!",
            "<!--",
        ]);
}

// Keep in sync with order of patterns in CONTENT_TYPE_PATTERN.
static CONTENT_TYPE_FROM_PATTERN: &'static [ContentType] =
    &[OpeningTag, ClosingTag, Instruction, Bang, Comment];

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
    let mut text_len = 0;
    loop {
        let (text_len_add, mut typ) = match CONTENT_TYPE_PATTERN.find(&code.str()[text_len..]) {
            Some(m) => (m.start(), CONTENT_TYPE_FROM_PATTERN[m.pattern()]),
            None => (code.rem(), Text),
        };
        text_len += text_len_add;
        // Check using Parsing.md tag rules.
        if typ == OpeningTag || typ == ClosingTag {
            let name = peek_tag_name(code);
            if typ == OpeningTag {
                // If character after `<` is TAG_NAME_CHAR, we're at an opening tag.
                // Otherwise, the `<` is interpreted literally as part of text.
                if name.is_empty() {
                    text_len += 1;
                    continue;
                };
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
                    typ = ClosingTagForVoidElement;
                } else if !parent.is_empty() && parent == name.as_slice() {
                    // Closing tag mismatch, reinterpret as opening tag.
                    typ = OpeningTag;
                };
            };
        };
        if text_len > 0 {
            nodes.push(NodeData::Text {
                value: decode_entities(code.slice_and_shift(text_len), false),
            });
            text_len = 0;
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
            MalformedLeftChevronSlash => code.shift(match memrchr(b'>', code.str()) {
                Some(m) => m + 1,
                None => code.rem(),
            }),
            OmittedClosingTag => {
                closing_tag_omitted = true;
                break;
            }
            ClosingTagForVoidElement => drop(parse_tag(code)),
        };
    }
    debug_assert_eq!(text_len, 0);
    ParsedContent {
        children: nodes,
        closing_tag_omitted,
    }
}
