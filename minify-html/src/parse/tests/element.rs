use rustc_hash::FxHashMap;

use crate::ast::{AttrVal, ElementClosingTag, NodeData};
use crate::common::spec::tag::ns::Namespace;
use crate::common::spec::tag::EMPTY_SLICE;
use crate::parse::element::{parse_element, parse_tag, ParsedTag};
use crate::parse::Code;

fn val(v: &[u8]) -> AttrVal {
    AttrVal {
        value: v.to_vec(),
        quote: None,
    }
}

#[test]
fn test_parse_tag() {
    let mut code = Code::new(
        br###"<input type


				 =
			"password"  "a"  = "  b  "   :cd  /e /=fg 	= /\h /i/ /j/k/l m=n=o q==\r/s/ / t] = /u  / w=//>"###,
    );
    let tag = parse_tag(&mut code);
    assert_eq!(
        tag,
        ParsedTag {
            attributes: {
                let mut map = FxHashMap::<Vec<u8>, AttrVal>::default();
                map.insert(b"type".to_vec(), val(b"password"));
                map.insert(b"\"a\"".to_vec(), val(b"  b  "));
                map.insert(b":cd".to_vec(), val(b""));
                map.insert(b"e".to_vec(), val(b""));
                map.insert(b"=fg".to_vec(), val(b"/\\h"));
                map.insert(b"i".to_vec(), val(b""));
                map.insert(b"j".to_vec(), val(b""));
                map.insert(b"k".to_vec(), val(b""));
                map.insert(b"l".to_vec(), val(b""));
                map.insert(b"m".to_vec(), val(b"n=o"));
                map.insert(b"q".to_vec(), val(b"=\\r/s/"));
                map.insert(b"t]".to_vec(), val(b"/u"));
                map.insert(b"w".to_vec(), val(b"//"));
                map
            },
            name: b"input".to_vec(),
            self_closing: false,
        }
    );
}

#[test]
fn test_parse_element() {
    let mut code = Code::new(br#"<a b=\"c\"></a>"#);
    let elem = parse_element(&mut code, Namespace::Html, EMPTY_SLICE);
    assert_eq!(
        elem,
        NodeData::Element {
            attributes: {
                let mut map = FxHashMap::<Vec<u8>, AttrVal>::default();
                map.insert(b"b".to_vec(), val(br#"\"c\""#));
                map
            },
            children: vec![],
            closing_tag: ElementClosingTag::Present,
            name: b"a".to_vec(),
            namespace: Namespace::Html,
            next_sibling_element_name: Vec::new(),
        }
    );
}
