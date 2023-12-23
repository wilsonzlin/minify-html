use magnus::define_global_function;
use magnus::function;
use minify_html::minify as minify_html_native;
use minify_html::Cfg as CfgNative;

#[magnus::wrap(class = "Cfg")]
struct Cfg {
  do_not_minify_doctype: bool,
  ensure_spec_compliant_unquoted_attribute_values: bool,
  keep_closing_tags: bool,
  keep_html_and_head_opening_tags: bool,
  keep_spaces_between_attributes: bool,
  keep_comments: bool,
  minify_css: bool,
  minify_css_level_1: bool,
  minify_css_level_2: bool,
  minify_css_level_3: bool,
  minify_js: bool,
  remove_bangs: bool,
  remove_processing_instructions: bool,
}

fn minify_html(source: String, cfg: &Cfg) -> String {
  let out_code = minify_html_native(source.as_bytes(), &CfgNative {
    do_not_minify_doctype: cfg.do_not_minify_doctype,
    ensure_spec_compliant_unquoted_attribute_values: cfg
      .ensure_spec_compliant_unquoted_attribute_values,
    keep_closing_tags: cfg.keep_closing_tags,
    keep_html_and_head_opening_tags: cfg.keep_html_and_head_opening_tags,
    keep_spaces_between_attributes: cfg.keep_spaces_between_attributes,
    keep_comments: cfg.keep_comments,
    minify_css: cfg.minify_css,
    minify_css_level_1: cfg.minify_css_level_1,
    minify_css_level_2: cfg.minify_css_level_2,
    minify_css_level_3: cfg.minify_css_level_3,
    minify_js: cfg.minify_js,
    remove_bangs: cfg.remove_bangs,
    remove_processing_instructions: cfg.remove_processing_instructions,
  });
  String::from_utf8(out_code).unwrap()
}

#[magnus::init]
fn init() {
  define_global_function("minify_html", function!(minify_html, 2))
}
