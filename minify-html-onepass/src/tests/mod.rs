use std::str::from_utf8;

use crate::cfg::Cfg;
use crate::err::ErrorType;
use crate::{in_place, with_friendly_error};

pub fn eval_with_cfg(src: &'static [u8], expected: &'static [u8], cfg: &Cfg) {
    let mut code = src.to_vec();
    match with_friendly_error(&mut code, cfg) {
        Ok(len) => {
            assert_eq!(
                from_utf8(&code[..len]).unwrap(),
                from_utf8(expected).unwrap()
            );
        }
        Err(super::FriendlyError {
            code_context,
            message,
            ..
        }) => {
            println!("{}", message);
            println!("{}", code_context);
            assert!(false);
        }
    };
}

pub fn eval_with_js_min(src: &'static [u8], expected: &'static [u8]) -> () {
    let mut cfg = Cfg::new();
    cfg.minify_js = true;
    eval_with_cfg(src, expected, &cfg);
}

pub fn eval_with_css_min(src: &'static [u8], expected: &'static [u8]) -> () {
    let mut cfg = Cfg::new();
    cfg.minify_css = true;
    eval_with_cfg(src, expected, &cfg);
}

pub fn eval(src: &'static [u8], expected: &'static [u8]) {
    eval_with_cfg(src, expected, &Cfg::new());
}

fn eval_error(src: &'static [u8], expected: ErrorType) -> () {
    let mut code = src.to_vec();
    assert_eq!(
        in_place(
            &mut code,
            &Cfg {
                minify_js: false,
                minify_css: false,
            }
        )
        .unwrap_err()
        .error_type,
        expected
    );
}

#[test]
// NOTE: Keep inputs in sync with main variant. Outputs are different as main variant reorders attributes.
fn test_space_between_attrs_minification() {
    eval(
        b"<div a=\" \" b=\" \"></div>",
        b"<div a=\" \"b=\" \"></div>",
    );
    eval(b"<div a=' ' b=\" \"></div>", b"<div a=\" \"b=\" \"></div>");
    eval(
        b"<div a=&#x20 b=\" \"></div>",
        b"<div a=\" \"b=\" \"></div>",
    );
    eval(b"<div a=\"1\" b=\" \"></div>", b"<div a=1 b=\" \"></div>");
    eval(b"<div a='1' b=\" \"></div>", b"<div a=1 b=\" \"></div>");
    eval(b"<div a=\"a\"b=\"b\"></div>", b"<div a=a b=b></div>");
}

#[test]
fn test_unmatched_closing_tag() {
    eval_error(b"Hello</p>Goodbye", ErrorType::UnexpectedClosingTag);
    eval_error(b"Hello<br></br>Goodbye", ErrorType::UnexpectedClosingTag);
    eval_error(
        b"<div>Hello</p>Goodbye",
        ErrorType::ClosingTagMismatch {
            expected: "div".to_string(),
            got: "p".to_string(),
        },
    );
    eval_error(
        b"<ul><li>a</p>",
        ErrorType::ClosingTagMismatch {
            expected: "ul".to_string(),
            got: "p".to_string(),
        },
    );
    eval_error(
        b"<ul><li><rt>a</p>",
        ErrorType::ClosingTagMismatch {
            expected: "ul".to_string(),
            got: "p".to_string(),
        },
    );
    eval_error(
        b"<html><head><body><ul><li><rt>a</p>",
        ErrorType::ClosingTagMismatch {
            expected: "ul".to_string(),
            got: "p".to_string(),
        },
    );
}
