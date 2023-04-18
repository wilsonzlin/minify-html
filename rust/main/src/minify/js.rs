use crate::cfg::Cfg;
use crate::common::whitespace::trimmed;
use minify_js::{minify as minifier, Session, TopLevelMode};

pub fn minify_js(cfg: &Cfg, session: &mut Session, mode: TopLevelMode, out: &mut Vec<u8>, code: &[u8]) {
    if cfg.minify_js {
        session.reset();
        // TODO Write to `out` directly, but only if we can guarantee that the length will never exceed the input.
        let mut output = Vec::new();
        let result = minifier(session, mode, code, &mut output);
        // TODO Collect error as warning.
        if result.is_ok() && output.len() < code.len() {
            out.extend_from_slice(output.as_slice());
            return;
        };
    }
    out.extend_from_slice(trimmed(code));
}
