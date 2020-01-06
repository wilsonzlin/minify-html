use std::cmp::max;
use std::collections::HashMap;
use std::env;
use std::fs::File;
use std::io::Write;
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

#[derive(Debug)]
struct TrieStats {
    max_cluster_holes: usize,
    max_cluster_length: usize,
    max_clusters_single_node: usize,
    total_clusters: usize,
    total_leaves: usize,
    total_nodes: usize,
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
        assert!(current.value_as_code.is_none());
        current.value_as_code = Some(val);
    }

    fn _node_var_name(trie_name: &Vec<String>, node_id: usize) -> String {
        format!("{}_TRIE_NODE_{}", snake_case(trie_name), node_id)
    }

    fn _node_type_name(trie_name: &Vec<String>, node_id: usize) -> String {
        format!("{}TrieNode{}", camel_case(trie_name), node_id)
    }

    fn _build(&self, ai: &mut AutoIncrement, stats: &mut TrieStats, name: &Vec<String>, value_type: &str, out: &mut String) -> usize {
        let id = ai.next();
        let node_type_name = if self.children.is_empty() {
            format!("TrieLeafNode::<{}>", value_type)
        } else {
            TrieBuilderNode::_node_type_name(name, id)
        };
        let node_var_name = TrieBuilderNode::_node_var_name(name, id);

        let mut child_chars: Vec<char> = self.children.keys().map(|e| *e).collect();
        child_chars.sort();
        // Each cluster is a vector of pairs of child character and corresponding child node ID.
        let mut child_char_clusters: Vec<Vec<Option<(u8, usize)>>> = vec![];
        let mut last_char: Option<u32> = None;
        for c in child_chars {
            let p = c as u32;
            debug_assert!(p <= 0x7f);
            // Allow a maximum gap length of 3 between any two children.
            // Create a new vector if first char or last char is more than 3 character positions away.
            if last_char.filter(|last| last + 3 >= p).is_none() {
                child_char_clusters.push(Vec::new());
            } else {
                // Fill any gap with None values.
                for _ in last_char.unwrap()..p - 1 {
                    child_char_clusters.last_mut().unwrap().push(None);
                };
            };
            child_char_clusters.last_mut().unwrap().push(
                Some((c as u8, self.children.get(&c).unwrap()._build(ai, stats, name, value_type, out)))
            );
            last_char = Some(p);
        };
        stats.max_cluster_holes = max(stats.max_cluster_holes, child_char_clusters.iter().map(|c| c.iter().filter(|c| c.is_none()).count()).max().unwrap_or(0));
        stats.max_cluster_length = max(stats.max_cluster_length, child_char_clusters.iter().map(|c| c.len()).max().unwrap_or(0));
        stats.max_clusters_single_node = max(stats.max_clusters_single_node, child_char_clusters.len());
        stats.total_clusters += child_char_clusters.len();
        stats.total_leaves += self.children.is_empty() as usize;
        stats.total_nodes += 1;

        if !(self.children.is_empty()) {
            out.push_str(format!("struct {} {{\n", node_type_name).as_str());
            out.push_str(format!("\tvalue: Option<{}>,\n", value_type).as_str());
            for (cluster_no, cluster) in child_char_clusters.iter().enumerate() {
                if cluster.len() == 1 {
                    // Even though child node always exists, wrap in Option as return value for get_child is Option.
                    out.push_str(format!("\tcluster_{}: Option<&'static dyn ITrieNode<{}>>,\n", cluster_no, value_type).as_str());
                } else {
                    out.push_str(format!("\tcluster_{}: [Option<&'static dyn ITrieNode<{}>>; {}],\n", cluster_no, value_type, cluster.len()).as_str());
                };
            };
            out.push_str("}\n");

            // TODO Investigate Send + Sync.
            out.push_str(format!("unsafe impl Send for {} {{}}\n", node_type_name).as_str());
            out.push_str(format!("unsafe impl Sync for {} {{}}\n", node_type_name).as_str());
            out.push_str(format!("impl ITrieNode<{}> for {} {{\n", value_type, node_type_name).as_str());
            out.push_str(format!("\tfn get_value(&self) -> Option<{}> {{ self.value }}\n", value_type).as_str());

            let mut get_child_fn_branches: Vec<String> = Vec::new();
            for (cluster_no, cluster) in child_char_clusters.iter().enumerate() {
                if cluster.len() == 1 {
                    get_child_fn_branches.push(format!("if c == {} {{ self.cluster_{} }}", cluster.first().unwrap().unwrap().0, cluster_no));
                } else {
                    let min = cluster.first().unwrap().unwrap();
                    let max = cluster.last().unwrap().unwrap();
                    get_child_fn_branches.push(format!("if c >= {} && c <= {} {{ self.cluster_{}[(c - {}) as usize] }}", min.0, max.0, cluster_no, min.0));
                };
            };
            get_child_fn_branches.push("{ None }".to_string());
            let get_child_fn_code = get_child_fn_branches.join("\n\t\telse ");
            out.push_str(format!(
                "\tfn get_child(&self, {}c: u8) -> Option<&dyn ITrieNode<{}>> {{\n\t\t{}\n\t}}\n",
                // Prefix `c` parameter with underscore if unused to suppress compiler warnings.
                if child_char_clusters.is_empty() { "_" } else { "" },
                value_type,
                get_child_fn_code,
            ).as_str());
            out.push_str("}\n");
        };

        out.push_str(format!("static {}: &(dyn ITrieNode<{}> + Send + Sync) = &{} {{\n", node_var_name, value_type, node_type_name).as_str());
        out.push_str(format!("\tvalue: {},\n", match &self.value_as_code {
            Some(v) => format!("Some({})", v),
            None => "None".to_string(),
        }.as_str()).as_str());
        for (cluster_no, cluster) in child_char_clusters.iter().enumerate() {
            if cluster.len() == 1 {
                out.push_str(format!("\tcluster_{}: Some({}),\n", cluster_no, TrieBuilderNode::_node_var_name(name, cluster.first().unwrap().unwrap().1)).as_str());
            } else {
                out.push_str(format!(
                    "\tcluster_{}: [{}],\n", cluster_no, cluster
                        .iter()
                        .map(|child| match child {
                            Some((_, child_id)) => format!("Some({})", TrieBuilderNode::_node_var_name(name, *child_id)),
                            None => "None".to_string(),
                        })
                        .collect::<Vec<String>>().join(", "),
                ).as_str());
            };
        };
        out.push_str("};\n\n");

        id
    }

    fn build(&mut self, name: &str, value_type: &str) -> String {
        let name_words = name.split(' ').map(|w| w.to_string()).collect::<Vec<String>>();
        let mut code = String::new();
        let mut stats = TrieStats {
            max_cluster_holes: 0,
            max_cluster_length: 0,
            max_clusters_single_node: 0,
            total_clusters: 0,
            total_leaves: 0,
            total_nodes: 0,
        };
        let root_id = self._build(&mut AutoIncrement::new(), &mut stats, &name_words, value_type, &mut code);
        println!("{} {:?}", name, stats);
        // Make trie root public and use proper variable name.
        code.replace(
            format!("static {}:", TrieBuilderNode::_node_var_name(&name_words, root_id)).as_str(),
            format!("pub static {}:", snake_case(&name_words)).as_str(),
        )
    }
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

    format!("SinglePattern {{ seq: {}, table: &[{}] }}",
        create_byte_string_literal(pattern.as_bytes()),
        table.iter().map(|v| v.to_string()).collect::<Vec<String>>().join(", "))
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
    let mut trie_builder = TrieBuilderNode::new();
    for (rep, entity) in entities {
        if rep.as_bytes().len() < entity.characters.as_bytes().len() {
            // Since we're minifying in place, we need to guarantee we'll never write something longer than source.
            println!("Entity {} is shorter than decoded UTF-8 bytes, skipping...", rep);
        } else {
            trie_builder.add(&rep[1..], create_byte_string_literal(entity.characters.as_bytes()));
        };
    };
    // Generate trie code from builder.
    let trie_code = trie_builder.build("entity references", "&'static [u8]");

    // Write trie code to output Rust file.
    write_rs("entities", trie_code);
}

fn generate_patterns() {
    let patterns: HashMap<String, String> = read_json("patterns");

    for (name, pattern) in patterns {
        let mut code = String::new();
        code.push_str(format!("static {}: &SinglePattern = &{};", name, build_pattern(pattern)).as_str());
        write_rs(format!("pattern_{}", name).as_str(), code);
    };
}

#[derive(Serialize, Deserialize)]
struct Trie {
    value_type: String,
    values: HashMap<String, String>,
}

fn generate_tries() {
    let tries: HashMap<String, Trie> = read_json("tries");

    for (name, trie) in tries {
        let mut trie_builder = TrieBuilderNode::new();
        for (seq, value_code) in trie.values {
            trie_builder.add(seq.as_str(), value_code);
        };
        let trie_code = trie_builder.build(name.as_str(), trie.value_type.as_str());
        write_rs(format!("trie_{}", name).as_str(), trie_code);
    }
}

fn main() {
    generate_entities();
    generate_patterns();
    generate_tries();
}
