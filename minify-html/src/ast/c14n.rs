use crate::ast::ElementClosingTag;
use crate::ast::NodeData;
use aho_corasick::AhoCorasickBuilder;
use aho_corasick::MatchKind;
use minify_html_common::pattern::Replacer;
use once_cell::sync::Lazy;
use std::io::Write;

static TEXT_REPLACER: Lazy<Replacer> = Lazy::new(|| {
  Replacer::new(
    AhoCorasickBuilder::new()
      .match_kind(MatchKind::LeftmostLongest)
      .build(vec![b"&".to_vec(), b"<".to_vec()])
      .unwrap(),
    vec![b"&amp;".to_vec(), b"&lt;".to_vec()],
  )
});
static DOUBLE_QUOTED_REPLACER: Lazy<Replacer> = Lazy::new(|| {
  Replacer::new(
    AhoCorasickBuilder::new()
      .match_kind(MatchKind::LeftmostLongest)
      .build(vec![b"&".to_vec(), b"\"".to_vec()])
      .unwrap(),
    vec![b"&amp;".to_vec(), b"&#34;".to_vec()],
  )
});
static SINGLE_QUOTED_REPLACER: Lazy<Replacer> = Lazy::new(|| {
  Replacer::new(
    AhoCorasickBuilder::new()
      .match_kind(MatchKind::LeftmostLongest)
      .build(vec![b"&".to_vec(), b"'".to_vec()])
      .unwrap(),
    vec![b"&amp;".to_vec(), b"&#39;".to_vec()],
  )
});
static UNQUOTED_REPLACER: Lazy<Replacer> = Lazy::new(|| {
  Replacer::new(
    AhoCorasickBuilder::new()
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
      ])
      .unwrap(),
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
  )
});

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
      attrs_sorted.sort_unstable_by(|a, b| a.0.cmp(b.0));
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
    NodeData::RcdataContent { typ: _, text } => {
      out.write_all(&TEXT_REPLACER.replace_all(text))?;
    }
    NodeData::ScriptOrStyleContent { code, .. } => {
      out.write_all(code)?;
    }
    NodeData::Text { value } => {
      out.write_all(&TEXT_REPLACER.replace_all(value))?;
    }
    NodeData::Opaque { raw_source } => {
      out.write_all(raw_source)?;
    }
  };
  Ok(())
}
