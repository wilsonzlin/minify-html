use crate::cfg::Cfg;

pub fn minify_doctype(_cfg: &Cfg, out: &mut Vec<u8>, legacy: &[u8], ended: bool) {
    out.extend_from_slice(b"<!doctypehtml");
    if !legacy.is_empty() {
        out.push(b' ');
        out.extend_from_slice(legacy);
    };
    if ended {
        out.extend_from_slice(b">");
    };
}
