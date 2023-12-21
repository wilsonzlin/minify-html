use crate::cfg::Cfg;
use crate::common::whitespace::trimmed;
use minify_js::{minify as minifier, TopLevelMode};

pub fn minify_js(cfg: &Cfg, mode: TopLevelMode, out: &mut Vec<u8>, code: &[u8]) {
    if cfg.minify_js {
        let source = code.to_vec();
        // TODO Write to `out` directly, but only if we can guarantee that the length will never exceed the input.
        let mut output = Vec::new();
        let result = minifier(mode, source, &mut output);
        // TODO Collect error as warning.
        if !result.is_err() && output.len() < code.len() {
            out.extend_from_slice(output.as_slice());
            return;
        };
    }
    out.extend_from_slice(trimmed(code));
}
