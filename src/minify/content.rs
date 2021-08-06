use aho_corasick::{AhoCorasickBuilder, MatchKind};
use lazy_static::lazy_static;

use crate::ast::{NodeData, ScriptOrStyleLang};
use crate::cfg::Cfg;
use crate::gen::codepoints::TAG_NAME_CHAR;
use crate::minify::bang::minify_bang;
use crate::minify::comment::minify_comment;
use crate::minify::css::minify_css;
use crate::minify::element::minify_element;
use crate::minify::instruction::minify_instruction;
use crate::minify::js::minify_js;
use crate::pattern::Replacer;
use crate::spec::entity::encode::encode_ampersands;
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
    };

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

pub fn minify_content(
    cfg: &Cfg,
    out: &mut Vec<u8>,
    // Use empty slice if none.
    parent: &[u8],
    nodes: &[NodeData],
) -> () {
    let mut index_of_last_text_or_elem_child = (nodes.len() as isize) - 1;
    while index_of_last_text_or_elem_child >= 0 {
        match nodes[index_of_last_text_or_elem_child as usize] {
            NodeData::Text { .. } | NodeData::Element { .. } => break,
            _ => index_of_last_text_or_elem_child -= 1,
        };
    };

    let mut previous_sibling_element: &[u8] = EMPTY_TAG_NAME;
    for (i, c) in nodes.iter().enumerate() {
        match c {
            NodeData::Bang { code, ended } => minify_bang(cfg, out, code, *ended),
            NodeData::Comment { code, ended } => minify_comment(cfg, out, code, *ended),
            NodeData::Element {
                attributes,
                children,
                closing_tag,
                name,
            } => {
                minify_element(
                    cfg,
                    out,
                    parent,
                    previous_sibling_element,
                    (i as isize) == index_of_last_text_or_elem_child,
                    name,
                    attributes,
                    *closing_tag,
                    children,
                );
                previous_sibling_element = name;
            }
            NodeData::Instruction { code, ended } => minify_instruction(cfg, out, code, *ended),
            NodeData::ScriptOrStyleContent { code, lang } => match lang {
                ScriptOrStyleLang::CSS => minify_css(cfg, out, code),
                ScriptOrStyleLang::Data => out.extend_from_slice(code),
                ScriptOrStyleLang::JS => minify_js(cfg, out, code),
            },
            NodeData::Text { value } => out.extend_from_slice(
                &CHEVRON_REPLACER.replace_all(
                    &encode_ampersands(value, false)
                )
            ),
        };
    };
}
