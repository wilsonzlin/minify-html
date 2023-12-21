use rustc_hash::FxHashMap;

use crate::ast::{AttrVal, ElementClosingTag, NodeData};
use crate::cfg::Cfg;
use crate::common::spec::tag::ns::Namespace;
use crate::common::spec::tag::omission::{can_omit_as_before, can_omit_as_last_node};
use crate::minify::attr::{minify_attr, AttrMinified};
use crate::minify::content::minify_content;

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
    attributes: FxHashMap<Vec<u8>, AttrVal>,
    closing_tag: ElementClosingTag,
    children: Vec<NodeData>,
) {
    // Output quoted attributes, followed by unquoted, to optimise space omission between attributes.
    let mut quoted = Vec::new();
    let mut unquoted = Vec::new();

    let is_meta_viewport = tag_name == b"meta"
        && attributes
            .get(b"name".as_ref())
            .filter(|a| a.value.eq_ignore_ascii_case(b"viewport"))
            .is_some();

    for (name, value) in attributes {
        match minify_attr(cfg, ns, tag_name, is_meta_viewport, &name, value.value) {
            AttrMinified::Redundant => {}
            a @ AttrMinified::NoValue => unquoted.push((name, a)),
            AttrMinified::Value(v) => {
                debug_assert!(v.len() > 0);
                if v.quoted() {
                    quoted.push((name, v));
                } else {
                    unquoted.push((name, AttrMinified::Value(v)));
                }
            }
        };
    }

    // Determinism.
    quoted.sort_unstable_by(|a, b| a.0.cmp(&b.0));
    unquoted.sort_unstable_by(|a, b| a.0.cmp(&b.0));

    // Attributes list could become empty after minification, so check opening tag omission eligibility after attributes minification.
    let can_omit_opening_tag = (tag_name == b"html" || tag_name == b"head")
        && quoted.len() + unquoted.len() == 0
        && !cfg.keep_html_and_head_opening_tags;
    let can_omit_closing_tag = !cfg.keep_closing_tags
        && (can_omit_as_before(tag_name, next_sibling_as_element_tag_name)
            || (is_last_child_text_or_element_node && can_omit_as_last_node(parent, tag_name)));

    if !can_omit_opening_tag {
        out.push(b'<');
        out.extend_from_slice(tag_name);

        for (i, (name, value)) in quoted.iter().enumerate() {
            if i == 0 || cfg.keep_spaces_between_attributes {
                out.push(b' ');
            };
            out.extend_from_slice(&name);
            out.push(b'=');
            debug_assert!(value.quoted());
            value.out(out);
        }
        for (i, (name, value)) in unquoted.iter().enumerate() {
            // Write a space between unquoted attributes,
            // or after the tag name if it wasn't written already during `quoted` processing,
            // or if forced by Cfg.
            if i > 0 || (i == 0 && quoted.len() == 0) || cfg.keep_spaces_between_attributes {
                out.push(b' ');
            };
            out.extend_from_slice(&name);
            if let AttrMinified::Value(v) = value {
                out.push(b'=');
                v.out(out);
            };
        }

        if closing_tag == ElementClosingTag::SelfClosing {
            // Write a space only if the last attribute is unquoted.
            if unquoted.len() > 0 {
                out.push(b' ');
            };
            out.push(b'/');
        };
        out.push(b'>');
    }

    if closing_tag == ElementClosingTag::SelfClosing || closing_tag == ElementClosingTag::Void {
        debug_assert!(children.is_empty());
        return;
    };

    minify_content(
        cfg,
        out,
        if tag_name == b"svg" {
            Namespace::Svg
        } else {
            ns
        },
        descendant_of_pre || (ns == Namespace::Html && tag_name == b"pre"),
        tag_name,
        children,
    );

    if closing_tag != ElementClosingTag::Present || can_omit_closing_tag {
        return;
    };
    out.extend_from_slice(b"</");
    out.extend_from_slice(tag_name);
    out.push(b'>');
}
