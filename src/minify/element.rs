use std::collections::HashMap;

use crate::ast::{ElementClosingTag, NodeData};
use crate::cfg::Cfg;
use crate::minify::attr::{minify_attr_val, AttrType};
use crate::minify::content::minify_content;
use crate::spec::tag::ns::Namespace;
use crate::spec::tag::omission::{can_omit_as_before, can_omit_as_last_node};

pub fn minify_element(
    cfg: &Cfg,
    out: &mut Vec<u8>,
    descendant_of_pre: bool,
    ns: Namespace,
    // Use an empty slice if none.
    parent: &[u8],
    // Use an empty slice if the next element or text sibling node is not an element.
    next_sibling_as_element_tag_name: &[u8],
    // If the last node of the parent is an element and it's this one.
    is_last_child_text_or_element_node: bool,
    tag_name: &[u8],
    attributes: HashMap<Vec<u8>, Vec<u8>>,
    closing_tag: ElementClosingTag,
    children: Vec<NodeData>,
) {
    let can_omit_closing_tag = cfg.omit_closing_tags
        && (can_omit_as_before(tag_name, next_sibling_as_element_tag_name)
            || (is_last_child_text_or_element_node && can_omit_as_last_node(parent, tag_name)));

    out.push(b'<');
    out.extend_from_slice(tag_name);
    let mut last_attr = AttrType::NoValue;
    let mut attrs_sorted = attributes.into_iter().collect::<Vec<_>>();
    attrs_sorted.sort_unstable_by(|a, b| a.0.cmp(&b.0));
    for (name, value) in attrs_sorted {
        let min = minify_attr_val(ns, tag_name, &name, value);
        if min.typ() == AttrType::Redundant {
            continue;
        };
        if !cfg.remove_spaces_between_attributes || last_attr != AttrType::Quoted {
            out.push(b' ');
        };
        out.extend_from_slice(&name);
        if min.len() == 0 {
            last_attr = AttrType::NoValue;
        } else {
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
