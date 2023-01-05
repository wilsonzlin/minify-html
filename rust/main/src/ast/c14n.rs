use std::io::Write;

use aho_corasick::{AhoCorasickBuilder, MatchKind};
use lazy_static::lazy_static;

use crate::ast::{ElementClosingTag, NodeData};
use crate::common::pattern::Replacer;

lazy_static! {
    static ref TEXT_REPLACER: Replacer = Replacer::new(
        AhoCorasickBuilder::new()
            .dfa(true)
            .match_kind(MatchKind::LeftmostLongest)
            .build(vec![b"&".to_vec(), b"<".to_vec(),]),
        vec![b"&amp;".to_vec(), b"&lt;".to_vec(),],
    );
    static ref DOUBLE_QUOTED_REPLACER: Replacer = Replacer::new(
        AhoCorasickBuilder::new()
            .dfa(true)
            .match_kind(MatchKind::LeftmostLongest)
            .build(vec![b"&".to_vec(), b"\"".to_vec(),]),
        vec![b"&amp;".to_vec(), b"&#34;".to_vec(),],
    );
    static ref SINGLE_QUOTED_REPLACER: Replacer = Replacer::new(
        AhoCorasickBuilder::new()
            .dfa(true)
            .match_kind(MatchKind::LeftmostLongest)
            .build(vec![b"&".to_vec(), b"'".to_vec(),]),
        vec![b"&amp;".to_vec(), b"&#39;".to_vec(),],
    );
    static ref UNQUOTED_REPLACER: Replacer = Replacer::new(
        AhoCorasickBuilder::new()
            .dfa(true)
            .match_kind(MatchKind::LeftmostLongest)
            .build(vec![
                b"&".to_vec(),
                b">".to_vec(),
                b"\"".to_vec(),
                b"'".to_vec(),
                b"\x09".to_vec(),
                b"\x0a".to_vec(),
                b"\x0c".to_vec(),
                b"\x0d".to_vec(),
                b"\x20".to_vec(),
            ]),
        vec![
            b"&amp;".to_vec(),
            b"&gt;".to_vec(),
            b"&#34;".to_vec(),
            b"&#39;".to_vec(),
            b"&#9;".to_vec(),
            b"&#10;".to_vec(),
            b"&#12;".to_vec(),
            b"&#13;".to_vec(),
            b"&#32;".to_vec(),
        ],
    );
}

pub fn c14n_serialise_ast<T: Write>(out: &mut T, node: &NodeData) -> std::io::Result<()> {
    match node {
        NodeData::Bang { code, .. } => {
            out.write_all(b"<!")?;
            out.write_all(code)?;
            out.write_all(b">")?;
        }
        NodeData::Comment { code, .. } => {
            out.write_all(b"<!--")?;
            out.write_all(code)?;
            out.write_all(b"-->")?;
        }
        NodeData::Doctype { legacy, .. } => {
            out.write_all(b"<!DOCTYPE html")?;
            if !legacy.is_empty() {
                out.write_all(b" ")?;
                out.write_all(legacy)?;
            };
            out.write_all(b">")?;
        }
        NodeData::Element {
            attributes,
            closing_tag,
            children,
            name,
            ..
        } => {
            out.write_all(b"<")?;
            out.write_all(name)?;
            let mut attrs_sorted = attributes.iter().collect::<Vec<_>>();
            attrs_sorted.sort_unstable_by(|a, b| a.0.cmp(&b.0));
            for (name, value) in attrs_sorted.iter() {
                out.write_all(b" ")?;
                out.write_all(name)?;
                if !value.value.is_empty() {
                    out.write_all(b"=")?;
                    match value.quote {
                        Some(b'"') => {
                            out.write_all(b"\"")?;
                            out.write_all(&DOUBLE_QUOTED_REPLACER.replace_all(&value.value))?;
                            out.write_all(b"\"")?;
                        }
                        Some(b'\'') => {
                            out.write_all(b"'")?;
                            out.write_all(&SINGLE_QUOTED_REPLACER.replace_all(&value.value))?;
                            out.write_all(b"'")?;
                        }
                        None => {
                            out.write_all(&UNQUOTED_REPLACER.replace_all(&value.value))?;
                        }
                        _ => unreachable!(),
                    };
                };
            }
            if closing_tag == &ElementClosingTag::SelfClosing {
                out.write_all(b" /")?;
            };
            out.write_all(b">")?;
            for c in children {
                c14n_serialise_ast(out, c)?;
            }
            if closing_tag == &ElementClosingTag::Present {
                out.write_all(b"</")?;
                out.write_all(name)?;
                out.write_all(b">")?;
            };
        }
        NodeData::Instruction { code, .. } => {
            out.write_all(b"<?")?;
            out.write_all(code)?;
            out.write_all(b"?>")?;
        }
        NodeData::RcdataContent { typ, text } => {
            out.write_all(&TEXT_REPLACER.replace_all(text))?;
        }
        NodeData::ScriptOrStyleContent { code, .. } => {
            out.write_all(code)?;
        }
        NodeData::Text { value } => {
            out.write_all(&TEXT_REPLACER.replace_all(value))?;
        }
    };
    Ok(())
}
