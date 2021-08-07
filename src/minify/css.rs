#[cfg(feature = "js-esbuild")]
use {
    crate::minify::esbuild::minify_using_esbuild,
    aho_corasick::{AhoCorasick, AhoCorasickBuilder},
    esbuild_rs::{Loader, TransformOptions, TransformOptionsBuilder},
    lazy_static::lazy_static,
    std::sync::Arc,
};

use crate::cfg::Cfg;

#[cfg(feature = "js-esbuild")]
lazy_static! {
    static ref STYLE_END: AhoCorasick = AhoCorasickBuilder::new()
        .ascii_case_insensitive(true)
        .build(&["</style"]);
    static ref TRANSFORM_OPTIONS: Arc<TransformOptions> = {
        let mut builder = TransformOptionsBuilder::new();
        builder.loader = Loader::CSS;
        builder.minify_identifiers = true;
        builder.minify_syntax = true;
        builder.minify_whitespace = true;
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
        minify_using_esbuild(out, code, &TRANSFORM_OPTIONS.clone(), &STYLE_END);
    }
}
