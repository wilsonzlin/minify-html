use crate::cfg::Cfg;
use crate::parse::Code;

mod ast;
mod cfg;
mod gen;
mod parse;
mod pattern;
mod spec;
mod tests;

/// Copies a slice into a new Vec and minifies it, returning the Vec.
/// The resulting Vec will only contain minified code.
///
/// # Arguments
///
/// * `code` - A slice of bytes representing the source code to minify.
/// * `cfg` - Configuration object to adjust minification approach.
///
/// # Examples
///
/// ```
/// use minify_html::{Cfg, minify};
///
/// let mut code: &[u8] = b"<p>  Hello, world!  </p>";
/// let cfg = &Cfg {
///     minify_js: false,
///     minify_css: false,
/// };
/// let minified = minify(&code, cfg);
/// assert_eq!(minified, b"<p>Hello, world!".to_vec());
/// ```
pub fn minify(code: &[u8], cfg: &Cfg) -> Vec<u8> {
    let code = Code::new(code);
    // TODO
    Vec::new()
}
