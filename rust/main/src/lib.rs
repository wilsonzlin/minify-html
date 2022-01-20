use std::io::Write;

use crate::ast::c14n::c14n_serialise_ast;
pub use crate::cfg::Cfg;
use crate::common::spec::tag::ns::Namespace;
use crate::common::spec::tag::EMPTY_SLICE;
use crate::minify::content::minify_content;
use crate::minify::css::minify_css as minify_css_internal;
use crate::minify::js::minify_js as minify_js_internal;
use crate::parse::content::parse_content;
use crate::parse::Code;

mod ast;
mod cfg;
mod common;
mod entity;
mod minify;
mod parse;
#[cfg(test)]
mod tests;

/// Minifies UTF-8 HTML code, represented as an array of bytes.
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
/// let mut cfg = Cfg::new();
/// cfg.keep_comments = true;
/// let minified = minify(&code, &cfg);
/// assert_eq!(minified, b"<p>Hello, world!".to_vec());
/// ```
pub fn minify(src: &[u8], cfg: &Cfg) -> Vec<u8> {
    let mut code = Code::new(src);
    let parsed = parse_content(&mut code, Namespace::Html, EMPTY_SLICE, EMPTY_SLICE);
    let mut out = Vec::with_capacity(src.len());
    minify_content(
        cfg,
        &mut out,
        Namespace::Html,
        false,
        EMPTY_SLICE,
        parsed.children,
    );
    out
}

/// Minifies UTF-8 CSS code, represented as an array of bytes.
///
/// [esbuild-rs](https://github.com/wilsonzlin/esbuild-rs) is used to minify
/// the CSS. The `js-esbuild` feature must be enabled, and cfg.minify_css must
/// be `true`; otherwise, this function has no effect.
///
/// # Arguments
///
/// * `code` - A slice of bytes representing the source code to minify.
/// * `cfg` - Configuration object to adjust minification approach.
///
/// # Examples
///
/// ```
/// use minify_html::{Cfg, minify_css};
///
/// let code: &[u8] = b"a {  color: red;  }";
/// let mut cfg = Cfg::new();
/// cfg.minify_css = true;
/// let minified = minify_css(&code, &cfg);
/// assert!(minified.len() < code.len());
/// ```
pub fn minify_css(src: &[u8], cfg: &Cfg) -> Vec<u8> {
    let mut out = Vec::with_capacity(src.len());
    minify_css_internal(cfg, &mut out, src);
    out
}

/// Minifies UTF-8 JavaScript code, represented as an array of bytes.
///
/// [esbuild-rs](https://github.com/wilsonzlin/esbuild-rs) is used to minify
/// the JS. The `js-esbuild` feature must be enabled, and cfg.minify_js must
/// be `true`; otherwise, this function has no effect.
///
/// # Arguments
///
/// * `code` - A slice of bytes representing the source code to minify.
/// * `cfg` - Configuration object to adjust minification approach.
///
/// # Examples
///
/// ```
/// use minify_html::{Cfg, minify_js};
///
/// let code: &[u8] = b"alert(  'Hello, world!'  );";
/// let mut cfg = Cfg::new();
/// cfg.minify_js = true;
/// let minified = minify_js(&code, &cfg);
/// assert!(minified.len() < code.len());
/// ```
pub fn minify_js(src: &[u8], cfg: &Cfg) -> Vec<u8> {
    let mut out = Vec::with_capacity(src.len());
    minify_js_internal(cfg, &mut out, src);
    out
}

pub fn canonicalise<T: Write>(out: &mut T, src: &[u8]) -> std::io::Result<()> {
    let mut code = Code::new(src);
    let parsed = parse_content(&mut code, Namespace::Html, EMPTY_SLICE, EMPTY_SLICE);
    for c in parsed.children {
        c14n_serialise_ast(out, &c)?;
    }
    Ok(())
}
