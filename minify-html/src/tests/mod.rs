use std::str::from_utf8;

use crate::cfg::Cfg;
use crate::minify;

pub fn eval_with_cfg(src: &'static [u8], expected: &'static [u8], cfg: &Cfg) {
    let min = minify(&src, cfg);
    assert_eq!(from_utf8(&min).unwrap(), from_utf8(expected).unwrap(),);
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
    let mut cfg = Cfg::new();
    // Most common tests assume the following minifications aren't done.
    cfg.keep_html_and_head_opening_tags = true;
    eval_with_cfg(src, expected, &cfg);
}

fn eval_without_keep_html_head(src: &'static [u8], expected: &'static [u8]) -> () {
    eval_with_cfg(src, expected, &Cfg::new());
}

#[test]
fn test_minification_of_doctype() {
    eval(b"<!DOCTYPE html><html>", b"<!doctypehtml><html>");
    eval(
        b"<!DOCTYPE html SYSTEM 'about:legacy-compat'><html>",
        b"<!doctypehtml SYSTEM 'about:legacy-compat'><html>",
    );
}

#[test]
fn test_parsing_extra_head_tag() {
    // Extra `<head>` in `<label>` should be dropped, so whitespace around `<head>` should be joined and therefore trimmed due to `<label>` whitespace rules.
    eval(
        b"<html><head><meta><head><link><head><body><label>  <pre> </pre> <head>  </label>",
        b"<html><head><meta><link><body><label><pre> </pre></label>",
    );
    // Same as above except it's a `</head>`, which should get reinterpreted as a `<head>`.
    eval(
        b"<html><head><meta><head><link><head><body><label>  <pre> </pre> </head>  </label>",
        b"<html><head><meta><link><body><label><pre> </pre></label>",
    );
    // `<head>` gets implicitly closed by `<body>`, so any following `</head>` should be ignored. (They should be anyway, since `</head>` would not be a valid closing tag.)
    eval(
        b"<html><head><body><label> </head> </label>",
        b"<html><head><body><label></label>",
    );
}

#[test]
fn test_removal_of_html_and_head_opening_tags() {
    // Even though `<head>` is dropped, it's still parsed, so its content is still subject to `<head>` whitespace minification rules.
    eval_without_keep_html_head(
        b"<!DOCTYPE html><html><head>  <meta> <body>",
        b"<!doctypehtml><meta><body>",
    );
    // The tag should not be dropped if it has attributes.
    eval_without_keep_html_head(
        b"<!DOCTYPE html><html lang=en><head>  <meta> <body>",
        b"<!doctypehtml><html lang=en><meta><body>",
    );
    // The tag should be dropped if it has no attributes after minification.
    eval_without_keep_html_head(
        b"<!DOCTYPE html><html style='  '><head>  <meta> <body>",
        b"<!doctypehtml><meta><body>",
    );
}

#[test]
fn test_unmatched_closing_tag() {
    eval(b"Hello</p>Goodbye", b"HelloGoodbye");
    eval(b"Hello<br></br>Goodbye", b"Hello<br>Goodbye");
    eval(b"<div>Hello</p>Goodbye", b"<div>HelloGoodbye");
    eval(b"<ul><li>a</p>", b"<ul><li>a");
    eval(b"<ul><li><rt>a</p>", b"<ul><li><rt>a");
    eval(
        b"<html><head><body><ul><li><rt>a</p>",
        b"<html><head><body><ul><li><rt>a",
    );
}

#[test]
// NOTE: Keep inputs in sync with onepass variant. Outputs are different as main variant reorders attributes.
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
    eval(b"<div a=\"1\" b=\" \"></div>", b"<div b=\" \"a=1></div>");
    eval(b"<div a='1' b=\" \"></div>", b"<div b=\" \"a=1></div>");
    eval(b"<div a=\"a\"b=\"b\"></div>", b"<div a=a b=b></div>");
}

#[test]
fn test_attr_whatwg_unquoted_value_minification() {
    let mut cfg = Cfg::new();
    cfg.ensure_spec_compliant_unquoted_attribute_values = true;
    eval_with_cfg(b"<a b==></a>", br#"<a b="="></a>"#, &cfg);
    eval_with_cfg(
        br#"<a b=`'"<<==/`/></a>"#,
        br#"<a b="`'&#34<<==/`/"></a>"#,
        &cfg,
    );
}

#[test]
fn test_alt_attr_minification() {
    eval(br#"<img alt="  ">"#, br#"<img alt="  ">"#);
    eval(br#"<img alt=" ">"#, br#"<img alt=" ">"#);
    eval(br#"<img alt="">"#, br#"<img alt>"#);
    eval(br#"<img alt=''>"#, br#"<img alt>"#);
    eval(br#"<img alt>"#, br#"<img alt>"#);
    eval(br#"<x-any-tag alt>"#, br#"<x-any-tag alt>"#);
}

#[test]
fn test_viewport_attr_minification() {
    eval(
        b"<meta name=viewport content='width=device-width, initial-scale=1'>",
        b"<meta content=width=device-width,initial-scale=1 name=viewport>",
    );
    let spec_compliant_cfg = Cfg::spec_compliant();
    eval_with_cfg(
        b"<meta name=viewport content='width=device-width, initial-scale=1'>",
        br#"<meta content="width=device-width,initial-scale=1" name=viewport>"#,
        &spec_compliant_cfg,
    );
}

#[test]
fn test_style_attr_minification() {
    eval_with_css_min(
        br#"<div style="color: yellow;"></div>"#,
        br#"<div style=color:yellow></div>"#,
    );
    // `style` attributes are removed if fully minified away.
    eval_with_css_min(br#"<div style="  /*  */   "></div>"#, br#"<div></div>"#);
}
