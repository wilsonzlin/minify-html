use crate::cfg::Cfg;
use crate::minify;
use minify_html_common::tests::create_common_css_test_data;
use minify_html_common::tests::create_common_js_test_data;
use minify_html_common::tests::create_common_noncompliant_test_data;
use minify_html_common::tests::create_common_test_data;
use std::str::from_utf8;

pub fn eval_with_cfg(src: &'static [u8], expected: &'static [u8], cfg: &Cfg) {
  let min = minify(&src, cfg);
  assert_eq!(from_utf8(&min).unwrap(), from_utf8(expected).unwrap(),);
}

pub fn eval_with_noncompliant(src: &'static [u8], expected: &'static [u8]) {
  let mut cfg = Cfg::new();
  cfg.enable_possibly_noncompliant();
  eval_with_cfg(src, expected, &cfg)
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
  cfg.allow_optimal_entities = true;
  eval_with_cfg(src, expected, &cfg);
}

// NOTE: This is different to `eval` as that enables `keep_html_and_head_opening_tags`.
fn eval_without_keep_html_head(src: &'static [u8], expected: &'static [u8]) -> () {
  eval_with_cfg(src, expected, &Cfg::new());
}

#[test]
fn test_common() {
  for (a, b) in create_common_test_data() {
    eval(a, b);
  }
  for (a, b) in create_common_noncompliant_test_data() {
    eval_with_noncompliant(a, b);
  }
  for (a, b) in create_common_css_test_data() {
    eval_with_css_min(a, b);
  }
  for (a, b) in create_common_js_test_data() {
    eval_with_js_min(a, b);
  }
}

#[test]
fn test_keep_ssi_comments() {
  eval(b"<!--#include >", b"");
  let mut cfg = Cfg::default();
  cfg.keep_ssi_comments = true;
  eval_with_cfg(b"<!--#include >", b"<!--#include >", &cfg);
}

#[test]
fn test_keep_input_type_text_attr() {
  eval(b"<input type=\"text\">", b"<input>");
  let mut cfg = Cfg::default();
  cfg.keep_input_type_text_attr = true;
  eval_with_cfg(b"<input type=\"TExt\">", b"<input type=text>", &cfg);
}

#[test]
fn test_preserve_template_brace_syntax() {
  eval_with_js_min(
    b"<p> {{   hello    world! %}  {%}{#} echo '  </p><P><script>  let x = 1; //'  }} </p>",
    b"<p>{{ hello world! %} {%}{#} echo '<p><script>let x=1",
  );
  let mut cfg = Cfg::default();
  cfg.preserve_brace_template_syntax = true;
  eval_with_cfg(
    b"<p> {{   hello    world! %}  {%}{#} echo '  </p><P><script>  let x = 1; //'  }} </p>",
    b"<p>{{   hello    world! %}  {%}{#} echo '  </p><P><script>  let x = 1; //'  }}",
    &cfg,
  );
  eval_with_cfg(
    b"<p> {%   hello    world! %}  {%}{#} echo '  </p><P><script>  let x = 1; //'  %} </p>",
    b"<p>{%   hello    world! %} {%}{#} echo '  </p><P><script>  let x = 1; //'  %}",
    &cfg,
  );
  eval_with_cfg(
    b"<p> {#   hello    world! #}  {#}{# echo '  </p><P><script>  let x = 1; //'  #} </p>",
    b"<p>{#   hello    world! #} {#}{# echo '  </p><P><script>  let x = 1; //'  #}",
    &cfg,
  );
}

#[test]
fn test_preserve_template_chevron_percent_syntax() {
  let mut cfg = Cfg::default();
  cfg.preserve_chevron_percent_template_syntax = true;
  eval_with_cfg(
    b"<p> <%   hello    world! #}  {#}{# echo '  </p><P><script>  let x = 1; //'  %> </p>",
    b"<p><%   hello    world! #}  {#}{# echo '  </p><P><script>  let x = 1; //'  %>",
    &cfg,
  );
}

#[test]
fn test_minification_of_doctype() {
  let mut cfg = Cfg::new();
  cfg.minify_doctype = true;
  eval_with_cfg(b"<!DOCTYPE html><div>", b"<!doctypehtml><div>", &cfg);
  eval_with_cfg(
    b"<!DOCTYPE html SYSTEM 'about:legacy-compat'><div>",
    b"<!doctypehtml SYSTEM 'about:legacy-compat'><div>",
    &cfg,
  );
}

#[test]
fn test_removal_of_empty_closing_tag() {
  eval(b"<body><p>1</><p>2</body>", b"<body><p>1<p>2");
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
    b"<!doctype html><meta><body>",
  );
  // The tag should not be dropped if it has attributes.
  eval_without_keep_html_head(
    b"<!DOCTYPE html><html lang=en><head>  <meta> <body>",
    b"<!doctype html><html lang=en><meta><body>",
  );
  // The tag should be dropped if it has no attributes after minification.
  eval_without_keep_html_head(
    b"<!DOCTYPE html><html style='  '><head>  <meta> <body>",
    b"<!doctype html><meta><body>",
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
  eval_with_noncompliant(
    b"<div a=\" \" b=\" \"></div>",
    b"<div a=\" \"b=\" \"></div>",
  );
  eval_with_noncompliant(b"<div a=' ' b=\" \"></div>", b"<div a=\" \"b=\" \"></div>");
  eval_with_noncompliant(
    b"<div a=&#x20 b=\" \"></div>",
    b"<div a=\" \"b=\" \"></div>",
  );
  eval_with_noncompliant(b"<div a=\"1\" b=\" \"></div>", b"<div b=\" \"a=1></div>");
  eval_with_noncompliant(b"<div a='1' b=\" \"></div>", b"<div b=\" \"a=1></div>");
  eval_with_noncompliant(b"<div a=\"a\"b=\"b\"></div>", b"<div a=a b=b></div>");
}

#[test]
fn test_attr_whatwg_unquoted_value_minification() {
  eval(b"<a b==></a>", br#"<a b="="></a>"#);
  eval(br#"<a b=`'"<<==/`/></a>"#, br#"<a b="`'&#34<<==/`/"></a>"#);
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
  eval_with_noncompliant(
    b"<meta name=viewport content='width=device-width, initial-scale=1'>",
    b"<meta content=width=device-width,initial-scale=1 name=viewport>",
  );
  eval(
    b"<meta name=viewport content='width=device-width, initial-scale=1'>",
    br#"<meta content="width=device-width,initial-scale=1" name=viewport>"#,
  );
}

#[test]
fn test_style_attr_minification() {
  eval_with_css_min(
    br#"<div style="color: yellow;"></div>"#,
    br#"<div style=color:#ff0></div>"#,
  );
  // `style` attributes are removed if fully minified away.
  eval_with_css_min(br#"<div style="  /*  */   "></div>"#, br#"<div></div>"#);
}
