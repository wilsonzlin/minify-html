use crate::cfg::Cfg;

pub fn minify_doctype(cfg: &Cfg, out: &mut Vec<u8>, legacy: &[u8], ended: bool) {
    out.extend_from_slice(b"<!doctype");
    if cfg.do_not_minify_doctype {
        out.push(b' ');
    }
    out.extend_from_slice(b"html");
    if !legacy.is_empty() {
        out.push(b' ');
        out.extend_from_slice(legacy);
    };
    if ended {
        out.extend_from_slice(b">");
    };
}
