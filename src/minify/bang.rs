use crate::cfg::Cfg;

pub fn minify_bang(cfg: &Cfg, out: &mut Vec<u8>, code: &[u8], ended: bool) {
    if !cfg.remove_bangs {
        out.extend_from_slice(b"<!");
        out.extend_from_slice(code);
        if ended {
            out.extend_from_slice(b">");
        };
    };
}
