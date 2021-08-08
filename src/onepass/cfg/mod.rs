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
}
