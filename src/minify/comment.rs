use crate::cfg::Cfg;

pub fn minify_comment(cfg: &Cfg, out: &mut Vec<u8>, code: &[u8], ended: bool) {
    if cfg.keep_comments {
        out.extend_from_slice(b"<!--");
        out.extend_from_slice(code);
        if ended {
            out.extend_from_slice(b"-->");
        };
    };
}
