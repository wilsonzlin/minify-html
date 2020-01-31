use std::collections::HashMap;
use std::env;
use std::fs::File;
use std::io::Write;
use std::ops::{Index, IndexMut};
use std::path::Path;

use serde::{Deserialize, Serialize};

fn create_byte_string_literal(bytes: &[u8]) -> String {
    format!("b\"{}\"", bytes
        .iter()
        .map(|&b| if b >= b' ' && b <= b'~' && b != b'\\' && b != b'"' {
            (b as char).to_string()
        } else {
            format!("\\x{:02x}", b)
        })
        .collect::<String>())
}

fn read_json<T>(name: &str) -> T where for<'de> T: Deserialize<'de> {
    let patterns_path = Path::new("gen").join(format!("{}.json", name));
    let patterns_file = File::open(patterns_path).unwrap();
    serde_json::from_reader(patterns_file).unwrap()
}

fn write_rs(name: &str, code: String) -> () {
    let out_dir = env::var("OUT_DIR").unwrap();
    let dest_path = Path::new(&out_dir).join(format!("gen_{}.rs", name));
    let mut dest_file = File::create(&dest_path).unwrap();
    dest_file.write_all(code.as_bytes()).unwrap();
}

fn name_words(n: &str) -> Vec<String> {
    n.split(' ').map(|w| w.to_string()).collect::<Vec<String>>()
}

fn snake_case(n: &Vec<String>) -> String {
    n.iter().map(|w| w.to_uppercase()).collect::<Vec<String>>().join("_")
}

fn camel_case(n: &Vec<String>) -> String {
    n.iter().map(|w| format!(
        "{}{}",
        w.as_bytes()[0].to_ascii_uppercase() as char,
        std::str::from_utf8(&w.as_bytes()[1..]).unwrap(),
    )).collect::<Vec<String>>().join("")
}

pub struct TwoDimensionalArray {
    data: Vec<usize>,
    cols: usize,
}

impl TwoDimensionalArray {
    pub fn new(rows: usize, cols: usize) -> TwoDimensionalArray {
        TwoDimensionalArray {
            data: vec![0usize; rows * cols],
            cols,
        }
    }

    pub fn prebuilt(data: Vec<usize>, cols: usize) -> TwoDimensionalArray {
        TwoDimensionalArray { data, cols }
    }
}

type TwoDimensionalArrayIndex = (usize, usize);

impl Index<TwoDimensionalArrayIndex> for TwoDimensionalArray {
    type Output = usize;

    fn index(&self, (row, col): TwoDimensionalArrayIndex) -> &Self::Output {
        &self.data[row * self.cols + col]
    }
}

impl IndexMut<TwoDimensionalArrayIndex> for TwoDimensionalArray {
    fn index_mut(&mut self, (row, col): TwoDimensionalArrayIndex) -> &mut Self::Output {
        &mut self.data[row * self.cols + col]
    }
}

fn build_pattern(pattern: String) -> String {
    assert!(pattern.is_ascii());
    let seq = pattern.as_bytes();
    let dfa = &mut TwoDimensionalArray::new(256, seq.len());

    dfa[(seq[0] as usize, 0)] = 1;
    let mut x = 0;
    let mut j = 1;
    while j < seq.len() {
        for c in 0..256 {
            dfa[(c, j)] = dfa[(c, x)];
        };
        dfa[(seq[j] as usize, j)] = j + 1;
        x = dfa[(seq[j] as usize, x)];
        j += 1;
    };

    format!(
        "crate::pattern::SinglePattern::prebuilt(&[{}], {})",
        dfa.data.iter().map(|v| v.to_string()).collect::<Vec<String>>().join(", "),
        seq.len(),
    )
}

#[derive(Serialize, Deserialize)]
struct TagAttr {
    boolean: bool,
    redundant_if_empty: bool,
    collapse_and_trim: bool,
    default_value: Option<String>,
}

impl TagAttr {
    fn code(&self) -> String {
        format!(r"
            AttributeMinification {{
                boolean: {boolean},
                redundant_if_empty: {redundant_if_empty},
                collapse_and_trim: {collapse_and_trim},
                default_value: {default_value},
            }}
        ",
            boolean = self.boolean,
            redundant_if_empty = self.redundant_if_empty,
            collapse_and_trim = self.collapse_and_trim,
            default_value = match &self.default_value {
                Some(val) => format!("Some({})", create_byte_string_literal(val.as_bytes())),
                None => "None".to_string(),
            },
        )
    }
}

fn generate_attr_map() {
    let attrs: HashMap<String, HashMap<String, HashMap<String, TagAttr>>> = read_json("attrs");
    let mut code = String::new();
    for (attr_name, namespaces) in attrs.iter() {
        let mut by_namespace_code = String::new();
        by_namespace_code.push_str(format!("static {}_ATTR: ByNamespace = ByNamespace {{\n", attr_name.to_uppercase()).as_str());
        for namespace in ["html".to_string(), "svg".to_string()].iter() {
            by_namespace_code.push_str(format!("\t{}: ", namespace).as_str());
            match namespaces.get(namespace) {
                None => by_namespace_code.push_str("None"),
                Some(tags_map) => {
                    if let Some(global_attr) = tags_map.get("*") {
                        code.push_str(format!(
                            "static {}_{}_ATTR: &AttrMapEntry = &AttrMapEntry::AllNamespaceElements({});\n\n",
                            namespace.to_uppercase(),
                            attr_name.to_uppercase(),
                            global_attr.code(),
                        ).as_str());
                    } else {
                        code.push_str(format!(
                            "static {}_{}_ATTR: &AttrMapEntry = &AttrMapEntry::SpecificNamespaceElements(phf::phf_map! {{\n{}\n}});\n\n",
                            namespace.to_uppercase(),
                            attr_name.to_uppercase(),
                            tags_map
                                .iter()
                                .map(|(tag_name, tag_attr)| format!(
                                    "b\"{}\" => {}",
                                    tag_name,
                                    tag_attr.code(),
                                ))
                                .collect::<Vec<String>>()
                                .join(",\n"),
                        ).as_str());
                    };
                    by_namespace_code.push_str(format!("Some({}_{}_ATTR)", namespace.to_uppercase(), attr_name.to_uppercase()).as_str());
                }
            };
            by_namespace_code.push_str(",\n");
        };
        by_namespace_code.push_str("};\n\n");
        code.push_str(&by_namespace_code);
    };
    code.push_str("pub static ATTRS: AttrMap = AttrMap::new(phf::phf_map! {\n");
    for attr_name in attrs.keys() {
        code.push_str(format!("\tb\"{}\" => {}_ATTR,\n", attr_name, attr_name.to_uppercase()).as_str());
    };
    code.push_str("});\n\n");
    write_rs("attrs", code);
}

#[derive(Serialize, Deserialize)]
struct Entity {
    codepoints: Vec<u32>,
    characters: String,
}

pub struct TrieBuilderNode {
    value: Option<String>,
    children: Vec<Option<TrieBuilderNode>>,
}

struct TrieBuilderGenerationContext<'t, 'v, 'o> {
    trie_name: &'t str,
    value_type: &'v str,
    next_id: usize,
    out: &'o mut String,
}

impl<'t, 'v, 'o> TrieBuilderGenerationContext<'t, 'v, 'o> {
    pub fn id(&mut self) -> usize {
        let next = self.next_id;
        self.next_id += 1;
        next
    }
}

impl TrieBuilderNode {
    pub fn new() -> TrieBuilderNode {
        let mut children = Vec::new();
        for _ in 0..256 {
            children.push(None);
        };
        TrieBuilderNode { value: None, children }
    }

    pub fn add(&mut self, seq: &[u8], value: String) -> () {
        let mut current = self;
        for c in seq.iter() {
            current = current.children[*c as usize].get_or_insert_with(|| TrieBuilderNode::new());
        };
        current.value.replace(value);
    }

    fn _generated_node_var_name(&self, trie_name: &str, node_id: usize) -> String {
        format!("{trie_name}_NODE_{node_id}", trie_name = trie_name, node_id = node_id)
    }

    fn _generate(&self, ctx: &mut TrieBuilderGenerationContext) -> usize {
        let children = self.children.iter().map(|c| match c {
            None => "None".to_string(),
            Some(c) => {
                let child_id = c._generate(ctx);
                format!("Some({})", self._generated_node_var_name(ctx.trie_name, child_id))
            }
        }).collect::<Vec<String>>().join(", ");
        let id = ctx.id();
        let code = format!(
            "static {var_name}: &'static crate::pattern::TrieNode<{value_type}> = &crate::pattern::TrieNode {{\n\tvalue: {value},\n\tchildren: [{children}],\n}};\n\n",
            var_name = self._generated_node_var_name(ctx.trie_name, id),
            value_type = ctx.value_type,
            value = self.value.as_ref().map_or("None".to_string(), |v| format!("Some({})", v)),
            children = children,
        );
        ctx.out.push_str(code.as_str());
        id
    }

    pub fn generate(&self, trie_name: &str, value_type: &str) -> String {
        let mut out = String::new();
        let mut ctx = TrieBuilderGenerationContext {
            trie_name,
            value_type,
            next_id: 0,
            out: &mut out,
        };
        let root_id = self._generate(&mut ctx);
        // Make root node public and use proper name.
        ctx.out.replace(
            format!("static {}", self._generated_node_var_name(trie_name, root_id)).as_str(),
            format!("pub static {}", trie_name).as_str()
        )
    }
}

fn generate_entities() {
    // Read named entities map from JSON file.
    let entities: HashMap<String, Entity> = read_json("entities");

    // Add entities to trie builder.
    let mut trie_builder: TrieBuilderNode = TrieBuilderNode::new();
    for (rep, entity) in entities {
        let val = if rep.as_bytes().len() < entity.characters.as_bytes().len() {
            // Since we're minifying in place, we need to guarantee we'll never write something longer than source.
            println!("Entity {} is shorter than decoded UTF-8 bytes...", rep);
            // Include '&' in value.
            create_byte_string_literal(rep.as_bytes())
        } else {
            create_byte_string_literal(entity.characters.as_bytes())
        };
        trie_builder.add(&(rep.as_bytes())[1..], val);
    };
    // Write trie code to output Rust file.
    write_rs("entities", trie_builder.generate(
        "ENTITY_REFERENCES",
        "&'static [u8]",
    ));
}

fn generate_patterns() {
    let patterns: HashMap<String, String> = read_json("patterns");

    for (name, pattern) in patterns {
        let mut code = String::new();
        code.push_str(format!("static {}: &crate::pattern::SinglePattern = &{};", name, build_pattern(pattern)).as_str());
        write_rs(format!("pattern_{}", name).as_str(), code);
    };
}

fn main() {
    generate_attr_map();
    generate_entities();
    generate_patterns();
}
