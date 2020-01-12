use std::collections::HashMap;
use std::env;
use std::fs::File;
use std::io::Write;
use std::path::Path;

use fastrie::{FastrieBuild, FastrieBuilderNode};
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
    n
        .iter()
        .map(|w| w.to_uppercase())
        .collect::<Vec<String>>()
        .join("_")
}

fn camel_case(n: &Vec<String>) -> String {
    n
        .iter()
        .map(|w| format!(
            "{}{}",
            w.as_bytes()[0].to_ascii_uppercase() as char,
            std::str::from_utf8(&w.as_bytes()[1..]).unwrap(),
        ))
        .collect::<Vec<String>>()
        .join("")
}

fn build_pattern(pattern: String) -> String {
    assert!(pattern.is_ascii());
    let seq = pattern.as_bytes();
    let mut max_prefix_len = 0usize;
    let mut table = vec![0usize; seq.len()];

    let mut i = 1;
    while i < seq.len() {
        if seq[i] == seq[max_prefix_len] {
            max_prefix_len += 1;
            table[i] = max_prefix_len;
            i += 1;
        } else {
            if max_prefix_len != 0 {
                max_prefix_len = table[max_prefix_len - 1];
            } else {
                table[i] = 0;
                i += 1;
            };
        };
    };

    format!("crate::pattern::SinglePattern {{ seq: {}, table: &[{}] }}",
        create_byte_string_literal(pattern.as_bytes()),
        table.iter().map(|v| v.to_string()).collect::<Vec<String>>().join(", "))
}

fn generate_fastrie_code(var_name: &str, value_type: &str, built: &FastrieBuild<String>) -> String {
    format!(r"
        pub static {var_name}: &fastrie::Fastrie<{value_type}> = &fastrie::Fastrie::<{value_type}>::from_prebuilt(
            &[{values}],
            &[{data}],
        );
    ",
        var_name = var_name,
        value_type = value_type,
        values = built.values.join(", "),
        data = built.data.iter().map(|v| v.to_string()).collect::<Vec<String>>().join(", "),
    )
}

fn generate_boolean_attrs() {
    let attrs: HashMap<String, Vec<String>> = read_json("boolean_attrs");
    let mut code = String::new();
    for (name, elems) in attrs.iter() {
        code.push_str(format!(
            "static {}_BOOLEAN_ATTR: &phf::Set<&'static [u8]> = &phf::phf_set!({});\n\n",
            name.to_uppercase(),
            elems.iter().map(|e| format!("b\"{}\"", e)).collect::<Vec<String>>().join(", "),
        ).as_str());
    };
    code.push_str("pub static BOOLEAN_ATTRS: phf::Map<&'static [u8], &'static phf::Set<&'static [u8]>> = phf::phf_map!{\n");
    for name in attrs.keys() {
        code.push_str(format!("\tb\"{}\" => {}_BOOLEAN_ATTR,\n", name, name.to_uppercase()).as_str());
    };
    code.push_str("};\n\n");
    write_rs("boolean_attrs", code);
}

#[derive(Serialize, Deserialize)]
struct Entity {
    codepoints: Vec<u32>,
    characters: String,
}

fn generate_entities() {
    // Read named entities map from JSON file.
    let entities: HashMap<String, Entity> = read_json("entities");

    // Add entities to trie builder.
    let mut trie_builder: FastrieBuilderNode<String> = FastrieBuilderNode::new();
    for (rep, entity) in entities {
        if rep.as_bytes().len() < entity.characters.as_bytes().len() {
            // Since we're minifying in place, we need to guarantee we'll never write something longer than source.
            println!("Entity {} is shorter than decoded UTF-8 bytes, skipping...", rep);
        } else {
            trie_builder.add(&(rep.as_bytes())[1..], create_byte_string_literal(entity.characters.as_bytes()));
        };
    };
    // Write trie code to output Rust file.
    write_rs("entities", generate_fastrie_code(
        "ENTITY_REFERENCES",
        "&'static [u8]",
        &trie_builder.prebuild(),
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

#[derive(Serialize, Deserialize)]
struct Trie {
    value_type: String,
    values: HashMap<String, String>,
}

fn generate_tries() {
    let tries: HashMap<String, Trie> = read_json("value_tries");

    for (name, trie) in tries {
        let mut trie_builder = FastrieBuilderNode::new();
        for (seq, value_code) in trie.values {
            trie_builder.add(seq.as_bytes(), value_code);
        };
        let var_name = snake_case(&name_words(name.as_str()));
        let trie_code = generate_fastrie_code(
            var_name.as_str(),
            trie.value_type.as_str(),
            &trie_builder.prebuild(),
        );
        write_rs(format!("trie_{}", var_name).as_str(), trie_code);
    };
}

fn main() {
    generate_boolean_attrs();
    generate_entities();
    generate_patterns();
    generate_tries();
}
