use aho_corasick::{AhoCorasickBuilder, MatchKind};
use lazy_static::lazy_static;

use crate::ast::{NodeData, ScriptOrStyleLang};
use crate::cfg::Cfg;
use crate::gen::codepoints::{TAG_NAME_CHAR, WHITESPACE};
use crate::minify::bang::minify_bang;
use crate::minify::comment::minify_comment;
use crate::minify::css::minify_css;
use crate::minify::element::minify_element;
use crate::minify::instruction::minify_instruction;
use crate::minify::js::minify_js;
use crate::pattern::Replacer;
use crate::spec::entity::encode::encode_ampersands;
use crate::spec::tag::ns::Namespace;
use crate::spec::tag::whitespace::{get_whitespace_minification_for_tag, WhitespaceMinification};
use crate::spec::tag::EMPTY_TAG_NAME;

fn build_chevron_replacer() -> Replacer {
    let mut patterns = Vec::<Vec<u8>>::new();
    let mut replacements = Vec::<Vec<u8>>::new();

    // Replace all `<` with a `&LT` if it's followed by a TAG_NAME_CHAR.
    for c in 0u8..128u8 {
        if TAG_NAME_CHAR[c] {
            patterns.push(vec![b'<', c]);
            replacements.push(vec![b'&', b'L', b'T', c]);
        };
    }

    Replacer::new(
        AhoCorasickBuilder::new()
            .dfa(true)
            .match_kind(MatchKind::LeftmostLongest)
            .build(patterns),
        replacements,
    )
}

lazy_static! {
    static ref CHEVRON_REPLACER: Replacer = build_chevron_replacer();
}

fn left_trim(val: &mut Vec<u8>) -> () {
    let mut len = 0;
    while val.get(len).filter(|&&c| WHITESPACE[c]).is_some() {
        len += 1;
    }
    val.drain(0..len);
}

fn right_trim(val: &mut Vec<u8>) -> () {
    let mut retain = val.len();
    while retain > 0 && val.get(retain - 1).filter(|&&c| WHITESPACE[c]).is_some() {
        retain -= 1;
    }
    val.truncate(retain);
}

fn collapse_whitespace(val: &mut Vec<u8>) -> () {
    let mut write = 0;
    let mut in_whitespace = false;
    for i in 0..val.len() {
        let mut c = val[i];
        if WHITESPACE[c] {
            if in_whitespace {
                // Skip this character.
                continue;
            };
            in_whitespace = true;
            c = b' ';
        };
        val[write] = c;
        write += 1;
    }
    val.truncate(write);
}

fn is_all_whitespace(val: &[u8]) -> bool {
    for &c in val {
        if !WHITESPACE[c] {
            return false;
        };
    }
    true
}

pub fn minify_content(
    cfg: &Cfg,
    out: &mut Vec<u8>,
    descendant_of_pre: bool,
    // Use empty slice if none.
    parent: &[u8],
    mut nodes: Vec<NodeData>,
) -> () {
    let &WhitespaceMinification {
        collapse,
        destroy_whole,
        trim,
    } = get_whitespace_minification_for_tag(parent, descendant_of_pre);

    // TODO Document or fix: even though bangs/comments/etc. don't affect layout, we don't collapse/destroy-whole/trim combined text nodes across bangs/comments/etc., as that's too complex and is ambiguous about which nodes should whitespace be deleted from.
    let mut found_first_text_or_elem = false;
    let mut index_of_last_nonempty_text_or_elem: isize = -1;
    let mut index_of_last_text_or_elem: isize = -1;
    for (i, n) in nodes.iter_mut().enumerate() {
        match n {
            NodeData::Element { .. } => {
                found_first_text_or_elem = true;
                index_of_last_nonempty_text_or_elem = i as isize;
                index_of_last_text_or_elem = i as isize;
            }
            NodeData::Text { value } => {
                if !found_first_text_or_elem {
                    // This is the first element or text node, and it's a text node.
                    found_first_text_or_elem = true;
                    if trim {
                        left_trim(value);
                    };
                };
                // Our parser is guaranteed to output contiguous text as a single node,
                // so the adjacent nodes to a text node (not counting comments/bangs/etc.) should be elements.
                // TODO debug_assert this and add tests.
                if destroy_whole && is_all_whitespace(value) {
                    value.clear();
                } else if collapse {
                    collapse_whitespace(value);
                };
                // Set AFTER processing.
                index_of_last_text_or_elem = i as isize;
                if !value.is_empty() {
                    index_of_last_nonempty_text_or_elem = i as isize;
                };
            }
            _ => {}
        };
    }
    if trim && index_of_last_text_or_elem > -1 {
        match nodes.get_mut(index_of_last_text_or_elem as usize).unwrap() {
            NodeData::Text { value } => right_trim(value),
            _ => {}
        };
    }

    let mut previous_sibling_element = Vec::<u8>::new();
    for (i, c) in nodes.into_iter().enumerate() {
        match c {
            NodeData::Bang { code, ended } => minify_bang(cfg, out, &code, ended),
            NodeData::Comment { code, ended } => minify_comment(cfg, out, &code, ended),
            NodeData::Element {
                attributes,
                children,
                closing_tag,
                name,
                namespace: child_ns,
            } => {
                minify_element(
                    cfg,
                    out,
                    descendant_of_pre,
                    child_ns,
                    parent,
                    &previous_sibling_element,
                    (i as isize) == index_of_last_nonempty_text_or_elem,
                    &name,
                    attributes,
                    closing_tag,
                    children,
                );
                previous_sibling_element = name;
            }
            NodeData::Instruction { code, ended } => minify_instruction(cfg, out, &code, ended),
            NodeData::ScriptOrStyleContent { code, lang } => match lang {
                ScriptOrStyleLang::CSS => minify_css(cfg, out, &code),
                ScriptOrStyleLang::Data => out.extend_from_slice(&code),
                ScriptOrStyleLang::JS => minify_js(cfg, out, &code),
            },
            NodeData::Text { value } => out.extend_from_slice(
                &CHEVRON_REPLACER.replace_all(&encode_ampersands(&value, false)),
            ),
        };
    }
}
