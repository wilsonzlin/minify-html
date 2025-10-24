use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
use crate::Cfg;
use aho_corasick::AhoCorasick;
use aho_corasick::AhoCorasickBuilder;
use lightningcss::stylesheet::MinifyOptions;
use lightningcss::stylesheet::ParserOptions;
use lightningcss::stylesheet::PrinterOptions;
use lightningcss::stylesheet::StyleSheet;
use once_cell::sync::Lazy;
use std::str::from_utf8_unchecked;

static STYLE_END: Lazy<AhoCorasick> = Lazy::new(|| {
  AhoCorasickBuilder::new()
    .ascii_case_insensitive(true)
    .build(["</style"])
    .unwrap()
});

#[inline(always)]
pub fn process_style(proc: &mut Processor, cfg: &Cfg) -> ProcessingResult<()> {
  proc.require_not_at_end()?;
  let src = proc.m(WhileNotSeq(&STYLE_END), Discard);
  // `process_tag` will require closing tag.

  if cfg.minify_css {
    let mut popt = PrinterOptions::default();
    popt.minify = true;
    let result = match StyleSheet::parse(
      unsafe { from_utf8_unchecked(&proc[src]) },
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
    if result.as_ref().filter(|r| r.len() < src.len()).is_some() {
      proc.write_slice(result.unwrap().as_bytes());
    } else {
      proc.write_range(src);
    };
  } else {
    proc.write_range(src);
  };

  Ok(())
}
