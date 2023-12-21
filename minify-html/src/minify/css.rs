use std::str::from_utf8_unchecked;

use crate::cfg::Cfg;
use crate::common::whitespace::trimmed;
use css_minify::optimizations::{Level, Minifier};

pub fn minify_css(cfg: &Cfg, out: &mut Vec<u8>, code: &[u8]) {
    if cfg.minify_css {
        let result = Minifier::default().minify(
            unsafe { from_utf8_unchecked(code) },
            if cfg.minify_css_level_1 {
                Level::One
            } else if cfg.minify_css_level_2 {
                Level::Two
            } else if cfg.minify_css_level_3 {
                Level::Three
            } else {
                // Default to One, as other levels may perform dangerous optimisations.
                Level::One
            },
        );
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
