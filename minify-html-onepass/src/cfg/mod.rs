/// Configuration settings that can be adjusted and passed to a minification function to change the
/// minification approach.
pub struct Cfg {
    /// If enabled, JavaScript in `<script>` tags are minified using
    /// [minify-js](https://github.com/wilsonzlin/minify-js).
    ///
    /// Only `<script>` tags with a valid or no
    /// [MIME type](https://mimesniff.spec.whatwg.org/#javascript-mime-type) is considered to
    /// contain JavaScript, as per the specification.
    pub minify_js: bool,

    /// If enabled, CSS in `<style>` tags are minified.
    pub minify_css: bool,
}

impl Cfg {
    pub fn new() -> Cfg {
        Cfg {
            minify_css: false,
            minify_js: false,
        }
    }
}
