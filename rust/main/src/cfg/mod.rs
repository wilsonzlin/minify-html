/// Configuration settings that can be adjusted and passed to a minification function to change the
/// minification approach.
#[derive(Default)]
pub struct Cfg {
    /// Do not minify DOCTYPEs. Minified DOCTYPEs may not be spec compliant.
    pub do_not_minify_doctype: bool,
    /// Ensure all unquoted attribute values in the output do not contain any characters prohibited by the [WHATWG specification](https://html.spec.whatwg.org/multipage/syntax.html#attributes-2).
    pub ensure_spec_compliant_unquoted_attribute_values: bool,
    /// Do not omit closing tags when possible.
    pub keep_closing_tags: bool,
    /// Do not omit `<html>` and `<head>` opening tags when they don't have attributes.
    pub keep_html_and_head_opening_tags: bool,
    /// Keep spaces between attributes when possible to conform to HTML standards.
    pub keep_spaces_between_attributes: bool,
    /// Keep all comments.
    pub keep_comments: bool,
    /// If enabled, CSS in `<style>` tags and `style` attributes are minified.
    pub minify_css: bool,
    /// If enabled, JavaScript in `<script>` tags are minified using
    /// [minify-js](https://github.com/wilsonzlin/minify-js).
    ///
    /// Only `<script>` tags with a valid or no
    /// [MIME type](https://mimesniff.spec.whatwg.org/#javascript-mime-type) is considered to
    /// contain JavaScript, as per the specification.
    pub minify_js: bool,
    /// Remove all bangs.
    pub remove_bangs: bool,
    /// Remove all processing_instructions.
    pub remove_processing_instructions: bool,
}

impl Cfg {
    pub fn new() -> Cfg {
        Cfg::default()
    }

    pub fn spec_compliant() -> Cfg {
        Cfg {
            do_not_minify_doctype: true,
            ensure_spec_compliant_unquoted_attribute_values: true,
            keep_spaces_between_attributes: true,
            ..Cfg::default()
        }
    }
}
