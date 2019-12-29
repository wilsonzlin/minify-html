use std::collections::HashMap;
use std::env;
use std::fs::File;
use std::io::Write;
use std::path::Path;

use serde::{Deserialize, Serialize};

struct AutoIncrement {
    next_val: usize,
}

impl AutoIncrement {
    fn new() -> AutoIncrement {
        AutoIncrement { next_val: 0 }
    }

    fn next(&mut self) -> usize {
        let v = self.next_val;
        self.next_val += 1;
        v
    }
}

struct TrieBuilderNode {
    children: HashMap<char, TrieBuilderNode>,
    value_as_code: Option<String>,
}

impl TrieBuilderNode {
    fn new() -> TrieBuilderNode {
        TrieBuilderNode {
            children: HashMap::new(),
            value_as_code: None,
        }
    }

    fn add(&mut self, pat: &str, val: String) -> () {
        let mut current = self;
        for c in pat.chars() {
            if !current.children.contains_key(&c) {
                current.children.insert(c, TrieBuilderNode::new());
            };
            current = current.children.get_mut(&c).unwrap();
        };
        current.value_as_code = Some(val);
    }

    fn build(&self, ai: &mut AutoIncrement, value_type: &'static str, out: &mut String) -> usize {
        let child_ids: Vec<(char, usize)> = self.children
            .iter()
            .map(|(&c, n)| (c, n.build(ai, value_type, out)))
            .collect();
        let id = ai.next();

        out.push_str(format!("static N{}: TrieNode<{}> = TrieNode::<{}> {{\n", id, value_type, value_type).as_str());
        out.push_str(format!("children: phf_map! {{\n").as_str());
        for (c, n) in child_ids {
            out.push_str(format!("b'{}' => &N{},\n", c, n).as_str());
        }
        out.push_str("},\n");
        out.push_str("value: ");
        match &self.value_as_code {
            Some(v) => {
                out.push_str(format!("Some({})", v).as_str());
            }
            None => out.push_str("None"),
        };
        out.push_str(",\n};\n");

        id
    }
}

#[derive(Serialize, Deserialize, Debug)]
struct Entity {
    codepoints: Vec<u32>,
    characters: String,
}

fn generate_entities() {
    let entities_path = Path::new("gen").join("entities.json");
    let entities_file = File::open(entities_path).unwrap();
    let entities: HashMap<String, Entity> = serde_json::from_reader(entities_file).unwrap();

    let mut trie_builder = TrieBuilderNode::new();
    for (rep, entity) in entities {
        trie_builder.add(&rep[1..], format!("b\"{}\"",
           entity.characters.as_bytes().iter().map(|b| format!("\\x{:02x}", b)).collect::<String>()
        ));
    }
    let mut trie_code = String::new();
    let trie_root_id = trie_builder.build(&mut AutoIncrement::new(), "&'static [u8]", &mut trie_code);

    let out_dir = env::var("OUT_DIR").unwrap();
    let dest_path = Path::new(&out_dir).join("gen_entities.rs");
    let mut dest_file = File::create(&dest_path).unwrap();

    dest_file.write_all(trie_code
        .replace(format!("static N{}:", trie_root_id).as_str(), "pub static ENTITY_REFERENCES:")
        .as_bytes()).unwrap();
}

fn main() {
    generate_entities();
}
