#[cfg(feature = "js-esbuild")]
use {
    crate::minify::esbuild::minify_using_esbuild,
    esbuild_rs::{Charset, LegalComments, Loader, SourceMap, TransformOptions, TransformOptionsBuilder},
    lazy_static::lazy_static,
    std::sync::Arc,
};

use crate::cfg::Cfg;

#[cfg(feature = "js-esbuild")]
lazy_static! {
    pub static ref MINIFY_CSS_TRANSFORM_OPTIONS: Arc<TransformOptions> = {
        let mut builder = TransformOptionsBuilder::new();
        builder.charset = Charset::UTF8;
        builder.legal_comments = LegalComments::None;
        builder.loader = Loader::CSS;
        builder.minify_identifiers = true;
        builder.minify_syntax = true;
        builder.minify_whitespace = true;
        builder.source_map = SourceMap::None;
        builder.build()
    };
}

#[cfg(not(feature = "js-esbuild"))]
pub fn minify_css(_cfg: &Cfg, out: &mut Vec<u8>, code: &[u8]) {
    out.extend_from_slice(&code);
}

#[cfg(feature = "js-esbuild")]
pub fn minify_css(cfg: &Cfg, out: &mut Vec<u8>, code: &[u8]) {
    if !cfg.minify_css {
        out.extend_from_slice(&code);
    } else {
        minify_using_esbuild(
            out,
            code,
            &MINIFY_CSS_TRANSFORM_OPTIONS.clone(),
        );
    }
}
