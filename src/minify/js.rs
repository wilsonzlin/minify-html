#[cfg(feature = "js-esbuild")]
use {
    crate::minify::esbuild::minify_using_esbuild,
    aho_corasick::{AhoCorasick, AhoCorasickBuilder},
    esbuild_rs::{TransformOptions, TransformOptionsBuilder},
    lazy_static::lazy_static,
    std::sync::Arc,
};

use crate::Cfg;

#[cfg(feature = "js-esbuild")]
lazy_static! {
    static ref SCRIPT_END: AhoCorasick = AhoCorasickBuilder::new()
        .ascii_case_insensitive(true)
        .build(&["</script"]);
    static ref TRANSFORM_OPTIONS: Arc<TransformOptions> = {
        let mut builder = TransformOptionsBuilder::new();
        builder.minify_identifiers = true;
        builder.minify_syntax = true;
        builder.minify_whitespace = true;
        builder.build()
    };
}

#[cfg(not(feature = "js-esbuild"))]
pub fn minify_js(_cfg: &Cfg, out: &mut Vec<u8>, code: &[u8]) {
    out.extend_from_slice(&code);
}

#[cfg(feature = "js-esbuild")]
pub fn minify_js(cfg: &Cfg, out: &mut Vec<u8>, code: &[u8]) {
    if !cfg.minify_js {
        out.extend_from_slice(&code);
    } else {
        minify_using_esbuild(out, code, &TRANSFORM_OPTIONS.clone(), &SCRIPT_END);
    }
}
