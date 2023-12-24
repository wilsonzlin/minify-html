use magnus::define_global_function;
use magnus::function;
use magnus::RHash;
use magnus::StaticSymbol;
use minify_html::minify as minify_html_native;
use minify_html::Cfg as CfgNative;

fn minify_html(source: String, cfg: RHash) -> String {
  #[rustfmt::skip]
  let out_code = minify_html_native(source.as_bytes(), &CfgNative {
    do_not_minify_doctype: cfg.aref(StaticSymbol::new("do_not_minify_doctype")).unwrap_or_default(),
    ensure_spec_compliant_unquoted_attribute_values: cfg.aref(StaticSymbol::new("ensure_spec_compliant_unquoted_attribute_values")).unwrap_or_default(),
    keep_closing_tags: cfg.aref(StaticSymbol::new("keep_closing_tags")).unwrap_or_default(),
    keep_html_and_head_opening_tags: cfg.aref(StaticSymbol::new("keep_html_and_head_opening_tags")).unwrap_or_default(),
    keep_spaces_between_attributes: cfg.aref(StaticSymbol::new("keep_spaces_between_attributes")).unwrap_or_default(),
    keep_comments: cfg.aref(StaticSymbol::new("keep_comments")).unwrap_or_default(),
    keep_ssi_comments: cfg.aref(StaticSymbol::new("keep_ssi_comments")).unwrap_or_default(),
    minify_css: cfg.aref(StaticSymbol::new("minify_css")).unwrap_or_default(),
    minify_js: cfg.aref(StaticSymbol::new("minify_js")).unwrap_or_default(),
    remove_bangs: cfg.aref(StaticSymbol::new("remove_bangs")).unwrap_or_default(),
    remove_processing_instructions: cfg.aref(StaticSymbol::new("remove_processing_instructions")).unwrap_or_default(),
  });
  String::from_utf8(out_code).unwrap()
}

#[magnus::init]
fn init() {
  define_global_function("minify_html", function!(minify_html, 2))
}
