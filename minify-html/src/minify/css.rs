use crate::cfg::Cfg;
#[cfg(feature = "css")]
use lightningcss::stylesheet::MinifyOptions;
#[cfg(feature = "css")]
use lightningcss::stylesheet::ParserOptions;
#[cfg(feature = "css")]
use lightningcss::stylesheet::PrinterOptions;
#[cfg(feature = "css")]
use lightningcss::stylesheet::StyleSheet;
use minify_html_common::whitespace::trimmed;
#[cfg(feature = "css")]
use std::str::from_utf8;

pub fn minify_css(cfg: &Cfg, out: &mut Vec<u8>, code: &[u8]) {
  if minify_css_with_lightningcss(cfg, out, code) {
    return;
  }
  out.extend_from_slice(trimmed(code));
}

/// Returns `true` and writes the minified code to `out` if minification was performed and
/// produced a smaller result; returns `false` (without touching `out`) otherwise, in which case
/// the caller falls back to the trimmed original code.
///
/// Without the `css` feature, this always returns `false`, i.e. behaves as if `cfg.minify_css`
/// were `false`.
#[cfg(feature = "css")]
fn minify_css_with_lightningcss(cfg: &Cfg, out: &mut Vec<u8>, code: &[u8]) -> bool {
  if !cfg.minify_css {
    return false;
  }
  let mut popt = PrinterOptions::default();
  popt.minify = true;
  let result = match StyleSheet::parse(
    from_utf8(code).expect("<style> content contains non-UTF-8"),
    ParserOptions::default(),
  ) {
    Ok(mut sty) => match sty.minify(MinifyOptions::default()) {
      Ok(()) => match sty.to_css(popt) {
        Ok(out) => Some(out.code),
        // TODO Collect error as warning.
        Err(_err) => None,
      },
      // TODO Collect error as warning.
      Err(_err) => None,
    },
    // TODO Collect error as warning.
    Err(_err) => None,
  };
  if let Some(min) = result {
    if min.len() < code.len() {
      out.extend_from_slice(min.as_bytes());
      return true;
    };
  };
  false
}

#[cfg(not(feature = "css"))]
fn minify_css_with_lightningcss(_cfg: &Cfg, _out: &mut Vec<u8>, _code: &[u8]) -> bool {
  false
}
