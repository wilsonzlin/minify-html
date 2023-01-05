use crate::cfg::Cfg;

pub fn minify_rcdata(cfg: &Cfg, out: &mut Vec<u8>, content: &[u8]) {
    out.extend_from_slice(content);
}
