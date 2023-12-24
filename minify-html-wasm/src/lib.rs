mod utils;

use js_sys::Reflect;
use wasm_bindgen::prelude::*;

// When the `wee_alloc` feature is enabled, use `wee_alloc` as the global
// allocator.
#[cfg(feature = "wee_alloc")]
#[global_allocator]
static ALLOC: wee_alloc::WeeAlloc = wee_alloc::WeeAlloc::INIT;

macro_rules! get_prop {
  ($cfg:expr, $x:literal) => {
    Reflect::get($cfg, &JsValue::from_str($x))
      .ok()
      .and_then(|p| p.as_bool())
      .unwrap_or(false)
  };
}

#[wasm_bindgen]
pub fn minify(code: &[u8], cfg: &JsValue) -> Vec<u8> {
  #[rustfmt::skip]
  let cfg = minify_html::Cfg {
    allow_noncompliant_unquoted_attribute_values: get_prop!(cfg, "allow_noncompliant_unquoted_attribute_values"),
    allow_removing_spaces_between_attributes: get_prop!(cfg, "allow_removing_spaces_between_attributes"),
    keep_closing_tags: get_prop!(cfg, "keep_closing_tags"),
    keep_comments: get_prop!(cfg, "keep_comments"),
    keep_html_and_head_opening_tags: get_prop!(cfg, "keep_html_and_head_opening_tags"),
    keep_input_type_text_attr: get_prop!(cfg, "keep_input_type_text_attr"),
    keep_ssi_comments: get_prop!(cfg, "keep_ssi_comments"),
    minify_css: get_prop!(cfg, "minify_css"),
    minify_doctype: get_prop!(cfg, "minify_doctype"),
    minify_js: get_prop!(cfg, "minify_js"),
    preserve_brace_template_syntax: get_prop!(cfg, "preserve_brace_template_syntax"),
    preserve_chevron_percent_template_syntax: get_prop!(cfg, "preserve_chevron_percent_template_syntax"),
    remove_bangs: get_prop!(cfg, "remove_bangs"),
    remove_processing_instructions: get_prop!(cfg, "remove_processing_instructions"),
  };
  minify_html::minify(code, &cfg)
}
