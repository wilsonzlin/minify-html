/// Configuration settings that can be adjusted and passed to a minification function to change the
/// minification approach.
pub struct Cfg {
    /// If enabled, JavaScript in `<script>` tags are minified using
    /// [esbuild-rs](https://github.com/wilsonzlin/esbuild-rs). The `js-esbuild` feature must be
    /// enabled; otherwise, this value has no effect.
    ///
    /// Only `<script>` tags with a valid or no
    /// [MIME type](https://mimesniff.spec.whatwg.org/#javascript-mime-type) is considered to
    /// contain JavaScript, as per the specification.
    pub minify_js: bool,

    /// If enabled, CSS in `<style>` tags are minified using
    /// [esbuild-rs](https://github.com/wilsonzlin/esbuild-rs). The `js-esbuild` feature must be
    /// enabled; otherwise, this value has no effect.
    pub minify_css: bool,

    /// Omit closing tags when possible.
    pub omit_closing_tags: bool,
    /// Remove spaces between attributes when possible (may result in invalid HTML).
    pub remove_spaces_between_attributes: bool,
    /// Remove all comments.
    pub remove_comments: bool,
    /// Remove all bangs.
    pub remove_bangs: bool,
    /// Remove all processing_instructions.
    pub remove_processing_instructions: bool,
}
