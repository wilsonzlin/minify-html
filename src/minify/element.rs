use std::collections::HashMap;

use crate::ast::{ElementClosingTag, NodeData, ScriptOrStyleLang};
use crate::cfg::Cfg;
use crate::gen::codepoints::TAG_NAME_CHAR;
use crate::minify::attr::{minify_attr_val, AttrType, AttrValMinified};
use crate::minify::bang::minify_bang;
use crate::minify::comment::minify_comment;
use crate::minify::content::minify_content;
use crate::minify::css::minify_css;
use crate::minify::instruction::minify_instruction;
use crate::minify::js::minify_js;
use crate::pattern::Replacer;
use crate::spec::entity::encode::encode_ampersands;
use crate::spec::tag::ns::Namespace;
use crate::spec::tag::omission::{can_omit_as_before, can_omit_as_last_node};
use crate::spec::tag::EMPTY_TAG_NAME;

pub fn minify_element(
    cfg: &Cfg,
    out: &mut Vec<u8>,
    descendant_of_pre: bool,
    ns: Namespace,
    // Use an empty slice if none.
    parent: &[u8],
    // Use an empty slice if none.
    previous_sibling_element: &[u8],
    // If the last node of the parent is an element and it's this one.
    is_last_child_text_or_element_node: bool,
    tag_name: &[u8],
    attributes: HashMap<Vec<u8>, Vec<u8>>,
    closing_tag: ElementClosingTag,
    children: Vec<NodeData>,
) -> () {
    let can_omit_closing_tag = cfg.omit_closing_tags
        && (can_omit_as_before(previous_sibling_element, tag_name)
            || (is_last_child_text_or_element_node && can_omit_as_last_node(parent, tag_name)));

    out.push(b'<');
    out.extend_from_slice(tag_name);
    let mut last_attr = AttrType::None;
    for (name, value) in attributes {
        if !cfg.remove_spaces_between_attributes || last_attr == AttrType::Unquoted {
            out.push(b' ');
        };
        out.extend_from_slice(&name);
        if !value.is_empty() {
            let min = minify_attr_val(&encode_ampersands(&value, true));
            out.push(b'=');
            min.out(out);
            last_attr = min.typ();
        };
    }
    if closing_tag == ElementClosingTag::SelfClosing {
        if last_attr == AttrType::Unquoted {
            out.push(b' ');
        };
        out.push(b'/');
    };
    out.push(b'>');

    if closing_tag == ElementClosingTag::SelfClosing || closing_tag == ElementClosingTag::Void {
        debug_assert!(children.is_empty());
        return;
    };

    minify_content(
        cfg,
        out,
        descendant_of_pre || (ns == Namespace::Html && tag_name == b"pre"),
        tag_name,
        children,
    );

    if closing_tag != ElementClosingTag::Present || (cfg.omit_closing_tags && can_omit_closing_tag)
    {
        return;
    };
    out.extend_from_slice(b"</");
    out.extend_from_slice(tag_name);
    out.push(b'>');
}
