/// Configuration settings that can be adjusted and passed to a minification function to change the
/// minification approach.
#[derive(Clone, Default)]
pub struct Cfg {
  /// Allow unquoted attribute values in the output to contain characters prohibited by the [WHATWG specification](https://html.spec.whatwg.org/multipage/syntax.html#attributes-2). These will still be parsed correctly by almost all browsers.
  pub allow_noncompliant_unquoted_attribute_values: bool,
  /// Allow some minifications around entities that may not pass validation, but will still be parsed correctly by almost all browsers.
  pub allow_optimal_entities: bool,
  /// Allow removing_spaces between attributes when possible, which may not be spec compliant. These will still be parsed correctly by almost all browsers.
  pub allow_removing_spaces_between_attributes: bool,
  /// Do not omit closing tags when possible.
  pub keep_closing_tags: bool,
  /// Keep all comments.
  pub keep_comments: bool,
  /// Do not omit `<html>` and `<head>` opening tags when they don't have attributes.
  pub keep_html_and_head_opening_tags: bool,
  /// Keep `type=text` attribute name and value on `<input>` elements.
  pub keep_input_type_text_attr: bool,
  /// Keep SSI comments.
  pub keep_ssi_comments: bool,
  /// Minify CSS in `<style>` tags and `style` attributes using [https://github.com/parcel-bundler/lightningcss](lightningcss).
  pub minify_css: bool,
  /// Minify DOCTYPEs. Minified DOCTYPEs may not be spec compliant, but will still be parsed correctly by almost all browsers.
  pub minify_doctype: bool,
  /// Minify JavaScript in `<script>` tags using [minify-js](https://github.com/wilsonzlin/minify-js).
  pub minify_js: bool,
  /// When `{{`, `{#`, or `{%` are seen in content, all source code until the subsequent matching closing `}}`, `#}`, or `%}` respectively gets piped through untouched.
  pub preserve_brace_template_syntax: bool,
  /// When `<%` is seen in content, all source code until the subsequent matching closing `%>` gets piped through untouched.
  pub preserve_chevron_percent_template_syntax: bool,
  /// Remove all bangs.
  pub remove_bangs: bool,
  /// Remove all processing instructions.
  pub remove_processing_instructions: bool,
}

impl Cfg {
  pub fn new() -> Cfg {
    Cfg::default()
  }

  pub fn enable_possibly_noncompliant(&mut self) {
    self.allow_noncompliant_unquoted_attribute_values = true;
    self.allow_optimal_entities = true;
    self.allow_removing_spaces_between_attributes = true;
    self.minify_doctype = true;
  }
}
