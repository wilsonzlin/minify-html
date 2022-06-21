use std::str::from_utf8_unchecked;

use crate::cfg::Cfg;
use crate::common::whitespace::trimmed;
use css_minify::optimizations::{Level, Minifier};

pub fn minify_css(cfg: &Cfg, out: &mut Vec<u8>, code: &[u8]) {
    if cfg.minify_css {
        let result = Minifier::default().minify(unsafe { from_utf8_unchecked(code) }, Level::Three);
        // TODO Collect error as warning.
        if let Ok(min) = result {
            if min.len() < code.len() {
                out.extend_from_slice(min.as_bytes());
                return;
            };
        };
    }
    out.extend_from_slice(trimmed(code));
}
