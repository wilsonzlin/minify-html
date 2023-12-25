use itertools::Itertools;
use serde::Deserialize;
use std::cell::RefCell;
use std::collections::HashMap;
use std::env::var_os;
use std::fmt::Write;
use std::ops::Add;
use std::ops::BitAnd;
use std::ops::BitOr;
use std::path::Path;
use std::rc::Rc;

#[derive(Deserialize, Hash, PartialEq, Eq, Clone, Copy)]
#[serde(rename_all = "lowercase")]
enum HtmlDataNamespace {
  Html,
  Svg,
}

#[derive(Deserialize)]
#[serde(rename_all = "camelCase")]
struct HtmlDataAttrConfig {
  #[serde(default)]
  boolean: bool,
  #[serde(default)]
  case_insensitive: bool,
  #[serde(default)]
  collapse: bool,
  /// Empty string is semantically different from None.
  default_value: Option<String>,
  #[serde(default)]
  redundant_if_empty: bool,
  #[serde(default)]
  trim: bool,
}

#[derive(Deserialize)]
#[allow(dead_code)]
struct HtmlData {
  tags: HashMap<HtmlDataNamespace, Vec<String>>,
  // attr => ns => tag => AttrConfig.
  attributes: HashMap<String, HashMap<HtmlDataNamespace, HashMap<String, HtmlDataAttrConfig>>>,
}

fn gen_attr_min_struct(
  HtmlDataAttrConfig {
    boolean,
    case_insensitive,
    collapse,
    default_value,
    redundant_if_empty,
    trim,
  }: &HtmlDataAttrConfig,
) -> String {
  let default_value = default_value
    .as_ref()
    .map(|v| format!(r#"Some(b"{v}")"#))
    .unwrap_or_else(|| "None".to_string());
  format!(
    r#"
      AttributeMinification {{
        boolean: {boolean},
        case_insensitive: {case_insensitive},
        collapse: {collapse},
        default_value: {default_value},
        redundant_if_empty: {redundant_if_empty},
        trim: {trim},
      }}
    "#
  )
}

fn gen_attrs_rs(html_data: &HtmlData) -> String {
  let mut code = r#"
    use once_cell::sync::Lazy;
    use ahash::AHashMap;
    use crate::spec::tag::ns::Namespace;

    pub struct AttributeMinification {
        pub boolean: bool,
        pub case_insensitive: bool,
        pub collapse: bool,
        pub default_value: Option<&'static [u8]>,
        pub redundant_if_empty: bool,
        pub trim: bool,
    }

    pub enum AttrMapEntry {
        AllNamespaceElements(AttributeMinification),
        SpecificNamespaceElements(AHashMap<&'static [u8], AttributeMinification>),
    }

    pub struct ByNamespace {
        // Make pub so this struct can be statically created in gen/attrs.rs.
        pub html: Option<AttrMapEntry>,
        pub svg: Option<AttrMapEntry>,
    }

    impl ByNamespace {
        fn get(&self, ns: Namespace) -> Option<&AttrMapEntry> {
            match ns {
                Namespace::Html => self.html.as_ref(),
                Namespace::Svg => self.svg.as_ref(),
            }
        }
    }

    pub struct AttrMap(AHashMap<&'static [u8], ByNamespace>);

    impl AttrMap {
        pub const fn new(map: AHashMap<&'static [u8], ByNamespace>) -> AttrMap {
            AttrMap(map)
        }

        pub fn get(&self, ns: Namespace, tag: &[u8], attr: &[u8]) -> Option<&AttributeMinification> {
            self.0.get(attr).and_then(|namespaces| namespaces.get(ns)).and_then(|entry| match entry {
                AttrMapEntry::AllNamespaceElements(min) => Some(min),
                AttrMapEntry::SpecificNamespaceElements(map) => map.get(tag),
            })
        }
    }

    pub static ATTRS: Lazy<AttrMap> = Lazy::new(|| {
      #[allow(unused_mut)]
      let mut m = AHashMap::<&'static [u8], ByNamespace>::default();
  "#.to_string();

  for (attr_name, namespaces) in html_data.attributes.iter() {
    write!(&mut code, r#"m.insert(b"{attr_name}", ByNamespace {{"#).unwrap();
    {
      for ns in [HtmlDataNamespace::Html, HtmlDataNamespace::Svg] {
        write!(&mut code, r#"{}:"#, match ns {
          HtmlDataNamespace::Html => "html",
          HtmlDataNamespace::Svg => "svg",
        })
        .unwrap();
        match namespaces.get(&ns) {
          None => write!(&mut code, "None").unwrap(),
          Some(tags_map) => match tags_map.get("*") {
            Some(global_attr) => write!(
              &mut code,
              r#"Some(AttrMapEntry::AllNamespaceElements({}))"#,
              gen_attr_min_struct(global_attr)
            )
            .unwrap(),
            None => {
              write!(
                &mut code,
                r#"
                  Some({{
                    #[allow(unused_mut)]
                    let mut m = AHashMap::<&'static [u8], AttributeMinification>::default();
                "#
              )
              .unwrap();
              for (tag_name, tag_attr) in tags_map {
                write!(
                  &mut code,
                  r#"m.insert(b"{}", {});"#,
                  tag_name,
                  gen_attr_min_struct(tag_attr)
                )
                .unwrap();
              }
              write!(
                &mut code,
                r#"
                  AttrMapEntry::SpecificNamespaceElements(m)
                }})
              "#
              )
              .unwrap();
            }
          },
        };
        write!(&mut code, ",").unwrap();
      }
    }
    write!(&mut code, "}});").unwrap();
  }
  write!(
    &mut code,
    r#"
        AttrMap::new(m)
      }});
    "#
  )
  .unwrap();
  code
}

#[derive(Clone, Copy)]
struct CodePoints([bool; 256]);

fn c(c: u8) -> CodePoints {
  CodePoints::new() & c
}

impl CodePoints {
  fn new() -> Self {
    Self([false; 256])
  }

  fn contains(&self, v: u8) -> bool {
    self.0[v as usize]
  }

  fn iter(&self) -> impl Iterator<Item = u8> + '_ {
    self
      .0
      .iter()
      .enumerate()
      .filter(|(_, e)| **e)
      .map(|(i, _)| i as u8)
  }
}

impl<I: IntoIterator<Item = u8>> Add<I> for CodePoints {
  type Output = Self;

  fn add(self, rhs: I) -> Self::Output {
    let mut lut = self.0;
    for v in rhs {
      lut[v as usize] = true;
    }
    CodePoints(lut)
  }
}

// I'd like to just use Add, but Rust doesn't allow implementing the same trait multiple times.
impl BitAnd<u8> for CodePoints {
  type Output = Self;

  fn bitand(self, rhs: u8) -> Self::Output {
    let mut lut = self.0;
    lut[rhs as usize] = true;
    CodePoints(lut)
  }
}

// I'd like to just use Add, but Rust doesn't allow implementing the same trait multiple times.
impl BitOr<CodePoints> for CodePoints {
  type Output = Self;

  fn bitor(self, rhs: CodePoints) -> Self::Output {
    let mut lut = self.0;
    for (i, v) in rhs.0.iter().enumerate() {
      if *v {
        lut[i] = true;
      }
    }
    CodePoints(lut)
  }
}

impl<I: IntoIterator<Item = u8>> From<I> for CodePoints {
  fn from(value: I) -> Self {
    CodePoints::new() + value
  }
}

// Official spec defined code points.
// See https://infra.spec.whatwg.org/#code-points for spec.
// TODO Full-system benchmark: are these LUTs actually faster than comparisons/branching? Esp. LUTs for less than 3 characters/comparisons.
fn gen_codepoints_rs() -> String {
  // Also update gen/tries.json when changing whitespace definition.
  let whitespace: CodePoints = [0x09u8, 0x0a, 0x0c, 0x0d, 0x20].into();
  let c0_control: CodePoints = (0u8..=0x1f).into();
  let control = c0_control + (0x7f..=0x9f);
  let digit: CodePoints = (b'0'..=b'9').into();
  let upper_hex_alpha: CodePoints = (b'A'..=b'F').into();
  let lower_hex_alpha: CodePoints = (b'a'..=b'f').into();
  let hex_digit = digit | upper_hex_alpha | lower_hex_alpha;
  let upper_alpha: CodePoints = (b'A'..=b'Z').into();
  let lower_alpha: CodePoints = (b'a'..=b'z').into();
  let alpha = upper_alpha | lower_alpha;
  let alphanumeric = digit | alpha;
  let alphanumeric_or_equals = digit | alpha & b'=';

  // Browsers are much more lax than the spec with regards to attribute names.
  // See https://html.spec.whatwg.org/multipage/syntax.html#syntax-attribute-name for spec.
  // To understand browser behaviour, try parsing:
  /*
  <input type


        =
      "password"  "a"  = "b"   :cd  /e /=fg 	= /\h /i/ /j/k/l m=n=o q==\r/s/ / t] = /u  / w=//>
  */
  let whitespace_or_slash = whitespace & b'/';
  let whitespace_or_slash_or_equals = whitespace_or_slash & b'=';
  let whitespace_or_slash_or_equals_or_right_chevron = whitespace_or_slash_or_equals & b'>';

  let double_quote = c(b'"');
  let single_quote = c(b'\'');
  // Official characters allowed in an attribute name.
  // NOTE: Unicode noncharacters not tested.
  // See https://html.spec.whatwg.org/multipage/syntax.html#syntax-attribute-name for spec.
  let whatwg_attr_name_char: CodePoints = (0..=255)
    .filter(|&c| match c {
      b' ' | b'"' | b'\'' | b'>' | b'/' | b'=' => false,
      c if control.contains(c) => false,
      _ => true,
    })
    .into();
  // Valid attribute quote characters.
  // See https://html.spec.whatwg.org/multipage/introduction.html#intro-early-example for spec.
  // Backtick is not a valid quote character according to spec.
  let attr_quote = double_quote | single_quote;
  // Valid unquoted attribute value characters.
  // See https://html.spec.whatwg.org/multipage/syntax.html#unquoted for spec.
  // Browsers seem to simply consider any characters until whitespace or `>` part of an unquoted attribute value, despite the spec having more restrictions on allowed characters.
  let not_unquoted_attr_val_char = whitespace & b'>';

  // Tag names may only use ASCII alphanumerics. However, some people also use `:` and `-`.
  // See https://html.spec.whatwg.org/multipage/syntax.html#syntax-tag-name for spec.
  let tag_name_char = alphanumeric & b':' & b'-';

  let mut code = r#"
    pub struct Lookup {
      table: [bool; 256],
    }

    impl std::ops::Index<u8> for Lookup {
      type Output = bool;

      fn index(&self, c: u8) -> &Self::Output {
        // `c` is definitely below 256 so it's always safe to directly index table without checking.
        unsafe {
          self.table.get_unchecked(c as usize)
        }
      }
    }
  "#
  .to_string();

  for (name, cp) in [
    ("WHITESPACE", whitespace),
    ("DIGIT", digit),
    ("UPPER_HEX_ALPHA", upper_hex_alpha),
    ("LOWER_HEX_ALPHA", lower_hex_alpha),
    ("HEX_DIGIT", hex_digit),
    ("ALPHANUMERIC_OR_EQUALS", alphanumeric_or_equals),
    ("WHITESPACE_OR_SLASH", whitespace_or_slash),
    (
      "WHITESPACE_OR_SLASH_OR_EQUALS_OR_RIGHT_CHEVRON",
      whitespace_or_slash_or_equals_or_right_chevron,
    ),
    ("WHATWG_ATTR_NAME_CHAR", whatwg_attr_name_char),
    ("DOUBLE_QUOTE", double_quote),
    ("SINGLE_QUOTE", single_quote),
    ("ATTR_QUOTE", attr_quote),
    ("NOT_UNQUOTED_ATTR_VAL_CHAR", not_unquoted_attr_val_char),
    ("TAG_NAME_CHAR", tag_name_char),
  ] {
    write!(
      &mut code,
      r#"
        pub static {name}: &Lookup = &Lookup {{
          table: {:?},
        }};
      "#,
      cp.0
    )
    .unwrap();
  }

  code
}

#[derive(Default)]
struct TrieNode {
  // Keys: 0..=255.
  children: HashMap<u8, Rc<RefCell<TrieNode>>>,
  value: Option<String>,
}

#[derive(Default)]
struct TrieBuilder {
  root: Rc<RefCell<TrieNode>>,
}

impl TrieBuilder {
  pub fn add(&mut self, pattern: Vec<CodePoints>, value: String) {
    let mut cur = vec![self.root.clone()];
    for cp in pattern {
      let mut next = vec![];
      for c in cp.iter() {
        for cur_node in cur.iter() {
          let mut cur_node_mut = cur_node.borrow_mut();
          let new_child = cur_node_mut.children.entry(c).or_default();
          next.push(new_child.clone());
        }
      }
      cur = next;
    }
    for n in cur {
      n.borrow_mut().value = Some(value.clone());
    }
  }

  pub fn generate(self, trie_name: &'static str, value_type: &'static str) -> String {
    #[derive(Default)]
    struct State {
      variables: Vec<String>,
      next_id: u64,
      code_cache: HashMap<String, String>,
      trie_name: &'static str,
      value_type: &'static str,
    }
    impl State {
      // Generate the code for a node's variable name and value, and return the name.
      fn generate_node(&mut self, node: Rc<RefCell<TrieNode>>) -> String {
        // Only generate defined children to cut down on size of array, which would otherwise
        // bog down compile time and binary size for large trees with lots of nodes.
        // If array is empty, just use zero.
        let first_idx = node.borrow().children.keys().cloned().min().unwrap_or(0);
        let children = (first_idx..=255)
          .map(|c| match node.borrow().children.get(&c) {
            Some(c) => format!("Some({})", self.generate_node(c.clone())),
            None => "None".to_string(),
          })
          .join(", ");

        let value = node
          .borrow()
          .value
          .as_ref()
          .map(|v| format!("Some({v})"))
          .unwrap_or_else(|| "None".to_string());
        let var_value = format!(
          r#"
            &crate::pattern::TrieNode {{
              offset: {first_idx},
              value: {value},
              children: &[{children}],
            }}
          "#
        );
        if let Some(existing_var_name) = self.code_cache.get(&var_value) {
          return existing_var_name.clone();
        };

        let id = self.next_id;
        self.next_id += 1;
        let name = format!("{}_NODE_{}", self.trie_name, id);
        let value_type = self.value_type;
        self.variables.push(format!(
          r#"
            static {name}: &crate::pattern::TrieNode<{value_type}> = {var_value};
          "#
        ));
        assert!(self.code_cache.insert(var_value, name.clone()).is_none());
        name
      }
    }
    let mut s = State {
      trie_name,
      value_type,
      ..Default::default()
    };
    let root_name = s.generate_node(self.root);
    // Make root node public and use proper name.
    s.variables.join("\n\n").replace(
      &format!("static {root_name}"),
      &format!("pub static {trie_name}"),
    )
  }
}

#[derive(Deserialize)]
#[allow(dead_code)]
struct Entity {
  codepoints: Vec<u32>,
  characters: String,
}

fn gen_entities_rs() -> String {
  let entities: HashMap<String, Entity> =
    serde_json::from_slice(include_bytes!("entities.json")).unwrap();
  let mut trie_builder = TrieBuilder::default();
  trie_builder.add(
    vec![c(b'&'), c(b'#'), (b'0'..=b'9').into()],
    "EntityType::Dec".to_string(),
  );
  trie_builder.add(
    vec![
      c(b'&'),
      c(b'#'),
      c(b'x'),
      CodePoints::new() + (b'0'..=b'9') + (b'a'..=b'f') + (b'A'..=b'F'),
    ],
    "EntityType::Hex".to_string(),
  );
  let mut shorter_encoded_entities = vec![];
  for (encoded, entity) in entities {
    let val = format!(r#"&{:?}"#, entity.characters.as_bytes());
    trie_builder.add(
      encoded
        .as_bytes()
        .iter()
        .map(|&c| CodePoints::new() & c)
        .collect_vec(),
      format!("EntityType::Named({})", val),
    );
    // We should encode if encoded is shorter than decoded.
    if encoded.len() < entity.characters.len() {
      shorter_encoded_entities.push((format!(r#"&{:?}"#, encoded.as_bytes()), val));
    };
  }

  let shorter_encoded_entities_encoded_rs = shorter_encoded_entities
    .iter()
    .map(|(e, _)| e)
    .join(",\n  ");
  let shorter_encoded_entities_decoded_rs = shorter_encoded_entities
    .iter()
    .map(|(_, d)| d)
    .join(",\n  ");
  let trie_rs = trie_builder.generate("ENTITY", "EntityType");
  format!(
    r#"
      pub static SHORTER_ENCODED_ENTITIES_ENCODED: &[&[u8]] = &[
        {shorter_encoded_entities_encoded_rs}
      ];
      pub static SHORTER_ENCODED_ENTITIES_DECODED: &[&[u8]] = &[
        {shorter_encoded_entities_decoded_rs}
      ];

      #[derive(Clone, Copy)]
      pub enum EntityType {{
        Named(&'static [u8]),
        Dec,
        Hex,
      }}

      {trie_rs}
    "#
  )
}

fn main() {
  let out_dir = var_os("OUT_DIR").unwrap();
  let out_dir = Path::new(&out_dir);

  let html_data: HtmlData =
    serde_json::from_slice(include_bytes!("html-data_2023013104.0.0.json")).unwrap();

  std::fs::write(out_dir.join("attrs.rs"), gen_attrs_rs(&html_data)).unwrap();
  std::fs::write(out_dir.join("codepoints.rs"), gen_codepoints_rs()).unwrap();
  std::fs::write(out_dir.join("entities.rs"), gen_entities_rs()).unwrap();

  println!("cargo:rerun-if-changed=build.rs");
}
