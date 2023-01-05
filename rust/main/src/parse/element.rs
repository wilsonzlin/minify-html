use rustc_hash::FxHashMap;

use crate::ast::{AttrVal, ElementClosingTag, NodeData, ScriptOrStyleLang};
use crate::common::gen::codepoints::{
    ATTR_QUOTE, DOUBLE_QUOTE, NOT_UNQUOTED_ATTR_VAL_CHAR, SINGLE_QUOTE, TAG_NAME_CHAR, WHITESPACE,
    WHITESPACE_OR_SLASH, WHITESPACE_OR_SLASH_OR_EQUALS_OR_RIGHT_CHEVRON,
};
use crate::common::spec::script::JAVASCRIPT_MIME_TYPES;
use crate::common::spec::tag::ns::Namespace;
use crate::common::spec::tag::void::VOID_TAGS;
use crate::entity::decode::decode_entities;
use crate::parse::content::{parse_content, ParsedContent};
use crate::parse::script::parse_script_content;
use crate::parse::style::parse_style_content;
use crate::parse::textarea::parse_textarea_content;
use crate::parse::title::parse_title_content;
use crate::parse::Code;
use std::fmt::{Debug, Formatter};
use std::str::from_utf8;

fn parse_tag_name(code: &mut Code) -> Vec<u8> {
    debug_assert!(code.as_slice().starts_with(b"<"));
    code.shift(1);
    code.shift_if_next(b'/');
    let mut name = code.copy_and_shift_while_in_lookup(TAG_NAME_CHAR);
    name.make_ascii_lowercase();
    name
}

pub fn peek_tag_name(code: &mut Code) -> Vec<u8> {
    let cp = code.take_checkpoint();
    let name = parse_tag_name(code);
    code.restore_checkpoint(cp);
    name
}

// Derive Eq for testing.
#[derive(Eq, PartialEq)]
pub struct ParsedTag {
    pub attributes: FxHashMap<Vec<u8>, AttrVal>,
    pub name: Vec<u8>,
    pub self_closing: bool,
}

impl Debug for ParsedTag {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        f.write_fmt(format_args!("<{}", from_utf8(&self.name).unwrap()))?;
        let mut attrs = self.attributes.iter().collect::<Vec<_>>();
        attrs.sort_unstable_by(|a, b| a.0.cmp(b.0));
        for (n, v) in attrs {
            f.write_fmt(format_args!(" {}={:?}", from_utf8(n).unwrap(), v))?;
        }
        if self.self_closing {
            f.write_str(" />")?;
        };
        std::fmt::Result::Ok(())
    }
}

// While not valid, attributes in closing tags still need to be parsed (and then discarded) as attributes e.g. `</div x=">">`, which is why this function is used for both opening and closing tags.
// TODO Use generics to create version that doesn't create a FxHashMap.
pub fn parse_tag(code: &mut Code) -> ParsedTag {
    let elem_name = parse_tag_name(code);
    let mut attributes = FxHashMap::default();
    let self_closing;
    loop {
        // At the beginning of this loop, the last parsed unit was either the tag name or an attribute (including its value, if it had one).
        let last = code.shift_while_in_lookup(WHITESPACE_OR_SLASH);
        if code.at_end() || code.shift_if_next(b'>') {
            self_closing = last.filter(|&c| c == b'/').is_some();
            // End of tag.
            break;
        };
        let mut attr_name = Vec::new();
        // An attribute name can start with `=`, but ends at the next whitespace, `=`, `/`, or `>`.
        if let Some(c) = code.shift_if_next_not_in_lookup(WHITESPACE_OR_SLASH) {
            attr_name.push(c);
        };
        attr_name.extend_from_slice(
            code.slice_and_shift_while_not_in_lookup(
                WHITESPACE_OR_SLASH_OR_EQUALS_OR_RIGHT_CHEVRON,
            ),
        );
        debug_assert!(!attr_name.is_empty());
        attr_name.make_ascii_lowercase();
        // See comment for WHITESPACE_OR_SLASH in codepoints.ts for details of complex attr parsing.
        code.shift_while_in_lookup(WHITESPACE);
        let has_value = code.shift_if_next(b'=');
        code.shift_while_in_lookup(WHITESPACE);
        let attr_value = if !has_value {
            AttrVal {
                quote: None,
                value: Vec::new(),
            }
        } else {
            // TODO Replace ATTR_QUOTE with direct comparison.
            let attr_delim = code.shift_if_next_in_lookup(ATTR_QUOTE);
            // It seems that for unquoted attribute values, if it's the last value in a tag and is immediately followed by `>`, any trailing `/` is NOT interpreted as a self-closing indicator and is always included as part of the value, even for SVG self-closable elements.
            let attr_delim_pred = match attr_delim {
                Some(b'"') => DOUBLE_QUOTE,
                Some(b'\'') => SINGLE_QUOTE,
                None => NOT_UNQUOTED_ATTR_VAL_CHAR,
                _ => unreachable!(),
            };
            let attr_value = decode_entities(
                code.slice_and_shift_while_not_in_lookup(attr_delim_pred),
                true,
            );
            if let Some(c) = attr_delim {
                // It might not be next if EOF (i.e. attribute value not closed).
                code.shift_if_next(c);
            };
            AttrVal {
                quote: attr_delim,
                value: attr_value,
            }
        };
        attributes.insert(attr_name, attr_value);
    }
    ParsedTag {
        attributes,
        name: elem_name,
        self_closing,
    }
}

// `<` must be next. `parent` should be an empty slice if it doesn't exist.
pub fn parse_element(code: &mut Code, ns: Namespace, parent: &[u8]) -> NodeData {
    let ParsedTag {
        name: elem_name,
        attributes,
        self_closing,
    } = parse_tag(code);

    // Embedded svg tags are immediately in the svg namespace and must be parsed as such.
    let ns = if elem_name == b"svg" {
        Namespace::Svg
    } else {
        ns
    };

    // Only foreign elements can be self closed.
    if self_closing && ns != Namespace::Html {
        return NodeData::Element {
            attributes,
            children: Vec::new(),
            closing_tag: ElementClosingTag::SelfClosing,
            name: elem_name,
            namespace: ns,
            next_sibling_element_name: Vec::new(),
        };
    };
    if VOID_TAGS.contains(elem_name.as_slice()) {
        return NodeData::Element {
            attributes,
            children: Vec::new(),
            closing_tag: ElementClosingTag::Void,
            name: elem_name,
            namespace: ns,
            next_sibling_element_name: Vec::new(),
        };
    };

    let ParsedContent {
        closing_tag_omitted,
        children,
    } = match (ns, elem_name.as_slice()) {
        (_, b"script") => match attributes.get(b"type".as_ref()) {
            Some(mime) if !JAVASCRIPT_MIME_TYPES.contains(mime.as_slice()) => {
                parse_script_content(code, ScriptOrStyleLang::Data)
            }
            Some(typ) if typ.as_slice() == b"module" => {
                parse_script_content(code, ScriptOrStyleLang::JSModule)
            }
            _ => parse_script_content(code, ScriptOrStyleLang::JS),
        },
        (_, b"style") => parse_style_content(code),
        (Namespace::Html, b"textarea") => parse_textarea_content(code),
        (Namespace::Html, b"title") => parse_title_content(code),
        _ => parse_content(code, ns, parent, &elem_name),
    };

    if !closing_tag_omitted {
        let closing_tag = parse_tag(code);
        debug_assert_eq!(closing_tag.name, elem_name);
    };

    NodeData::Element {
        attributes,
        children,
        closing_tag: if closing_tag_omitted {
            ElementClosingTag::Omitted
        } else {
            ElementClosingTag::Present
        },
        name: elem_name,
        namespace: ns,
        next_sibling_element_name: Vec::new(),
    }
}
