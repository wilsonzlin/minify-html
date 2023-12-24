use crate::cfg::Cfg;

pub fn minify_comment(cfg: &Cfg, out: &mut Vec<u8>, code: &[u8], ended: bool) {
  let is_ssi = code.starts_with(b"#");
  if cfg.keep_comments || (is_ssi && cfg.keep_ssi_comments) {
    out.extend_from_slice(b"<!--");
    out.extend_from_slice(code);
    if ended {
      out.extend_from_slice(b"-->");
    };
  };
}
