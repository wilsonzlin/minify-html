#[cfg(feature = "js-esbuild")]
use {
    crate::minify::esbuild::minify_using_esbuild,
    esbuild_rs::{Charset, LegalComments, SourceMap, TransformOptions, TransformOptionsBuilder},
    lazy_static::lazy_static,
    std::sync::Arc,
};

use crate::cfg::Cfg;
use crate::common::whitespace::trimmed;

#[cfg(feature = "js-esbuild")]
lazy_static! {
    static ref TRANSFORM_OPTIONS: Arc<TransformOptions> = {
        let mut builder = TransformOptionsBuilder::new();
        builder.charset = Charset::UTF8;
        builder.legal_comments = LegalComments::None;
        builder.minify_identifiers = true;
        builder.minify_syntax = true;
        builder.minify_whitespace = true;
        builder.source_map = SourceMap::None;
        builder.build()
    };
}

#[cfg(not(feature = "js-esbuild"))]
pub fn minify_js(_cfg: &Cfg, out: &mut Vec<u8>, code: &[u8]) {
    out.extend_from_slice(trimmed(code));
}

#[cfg(feature = "js-esbuild")]
pub fn minify_js(cfg: &Cfg, out: &mut Vec<u8>, code: &[u8]) {
    if !cfg.minify_js {
        out.extend_from_slice(trimmed(code));
    } else {
        minify_using_esbuild(out, code, &TRANSFORM_OPTIONS.clone());
    }
}
