use crate::cfg::Cfg;
use crate::minify::content::minify_content;
use crate::parse::Code;
use crate::parse::content::parse_content;
use crate::spec::tag::EMPTY_TAG_NAME;
use crate::spec::tag::ns::Namespace;

mod ast;
mod cfg;
mod gen;
mod minify;
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
pub fn minify(src: &[u8], cfg: &Cfg) -> Vec<u8> {
    let mut code = Code::new(src);
    let parsed = parse_content(cfg, &mut code, Namespace::Html, EMPTY_TAG_NAME, EMPTY_TAG_NAME);
    let mut out = Vec::with_capacity(src.len());
    minify_content(cfg, &mut out, EMPTY_TAG_NAME, &parsed.children);
    out
}
