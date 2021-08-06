use std::collections::HashMap;

use crate::ast::{ElementClosingTag, NodeData};
use crate::parse::element::{parse_element, parse_tag, ParsedTag};
use crate::parse::Code;
use crate::spec::tag::ns::Namespace;
use crate::spec::tag::EMPTY_TAG_NAME;

#[test]
fn test_parse_tag() {
    let mut code = Code::new(
        br###"<input type


				 =
			"password"  "a"  = "b"   :cd  /e /=fg 	= /\h /i/ /j/k/l m=n=o q==\r/s/ / t] = /u  / w=//>"###,
    );
    let tag = parse_tag(&mut code);
    assert_eq!(
        tag,
        ParsedTag {
            attributes: {
                let mut map = HashMap::<Vec<u8>, Vec<u8>>::new();
                map.insert(b"type".to_vec(), b"password".to_vec());
                map.insert(b"\"a\"".to_vec(), b"b".to_vec());
                map.insert(b":cd".to_vec(), b"".to_vec());
                map.insert(b"e".to_vec(), b"".to_vec());
                map.insert(b"=fg".to_vec(), b"/\\h".to_vec());
                map.insert(b"i".to_vec(), b"".to_vec());
                map.insert(b"j".to_vec(), b"".to_vec());
                map.insert(b"k".to_vec(), b"".to_vec());
                map.insert(b"l".to_vec(), b"".to_vec());
                map.insert(b"m".to_vec(), b"n=o".to_vec());
                map.insert(b"q".to_vec(), b"=\\r/s/".to_vec());
                map.insert(b"t]".to_vec(), b"/u".to_vec());
                map.insert(b"w".to_vec(), b"//".to_vec());
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
    let elem = parse_element(&mut code, Namespace::Html, EMPTY_TAG_NAME);
    assert_eq!(
        elem,
        NodeData::Element {
            attributes: {
                let mut map = HashMap::<Vec<u8>, Vec<u8>>::new();
                map.insert(b"b".to_vec(), br#"\"c\""#.to_vec());
                map
            },
            children: vec![],
            closing_tag: ElementClosingTag::Present,
            name: b"a".to_vec(),
            namespace: Namespace::Html,
        }
    );
}
