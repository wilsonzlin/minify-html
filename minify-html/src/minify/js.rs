use crate::cfg::Cfg;
use minify_html_common::whitespace::trimmed;
#[cfg(feature = "js")]
use oxc_allocator::Allocator;
#[cfg(feature = "js")]
use oxc_codegen::Codegen;
#[cfg(feature = "js")]
use oxc_codegen::CodegenOptions;
#[cfg(feature = "js")]
use oxc_codegen::CommentOptions;
#[cfg(feature = "js")]
use oxc_minifier::CompressOptions;
#[cfg(feature = "js")]
use oxc_minifier::MangleOptions;
#[cfg(feature = "js")]
use oxc_minifier::Minifier;
#[cfg(feature = "js")]
use oxc_minifier::MinifierOptions;
#[cfg(feature = "js")]
use oxc_parser::Parser;
#[cfg(feature = "js")]
use oxc_span::SourceType;

/// Represents the mode in which JavaScript should be parsed and minified
#[derive(Debug, Clone, Copy)]
pub enum TopLevelMode {
  /// Parse as a global script
  Global,
  /// Parse as an ES module
  Module,
}

pub fn minify_js(cfg: &Cfg, mode: TopLevelMode, out: &mut Vec<u8>, code: &[u8]) {
  if minify_js_with_oxc(cfg, mode, out, code) {
    return;
  }
  // Fall back to trimmed original code
  out.extend_from_slice(trimmed(code));
}

/// Returns `true` and writes the minified code to `out` if minification was performed and
/// produced a smaller result; returns `false` (without touching `out`) otherwise, in which case
/// the caller falls back to the trimmed original code.
///
/// Without the `js` feature, this always returns `false`, i.e. behaves as if `cfg.minify_js` were
/// `false`.
#[cfg(feature = "js")]
fn minify_js_with_oxc(cfg: &Cfg, mode: TopLevelMode, out: &mut Vec<u8>, code: &[u8]) -> bool {
  if !cfg.minify_js {
    return false;
  }
  // Try to convert bytes to UTF-8 string for parsing
  if let Ok(source_text) = std::str::from_utf8(code) {
    let allocator = Allocator::default();

    // Determine source type based on mode
    let source_type = match mode {
      TopLevelMode::Module => SourceType::mjs(),
      TopLevelMode::Global => SourceType::default(),
    };

    // Parse the JavaScript code
    let parser_ret = Parser::new(&allocator, source_text, source_type).parse();

    // Only proceed if parsing succeeded without errors
    if parser_ret.errors.is_empty() {
      let mut program = parser_ret.program;

      // Apply minification
      // Use CompressOptions::safest() instead of default() to avoid overly aggressive dead code elimination
      let _minifier_ret = Minifier::new(MinifierOptions {
        mangle: Some(MangleOptions::default()),
        compress: Some(CompressOptions::safest()),
      }).minify(&allocator, &mut program);

      // Generate minified code
      // Disable treeshake annotations (e.g., /*#__PURE__*/, /*@__PURE__*/)
      // These are only useful for bundlers, not inline scripts
      let codegen_options = CodegenOptions {
        minify: true,
        comments: CommentOptions {
          annotation: false,
          ..CommentOptions::default()
        },
        ..CodegenOptions::default()
      };
      let minified = Codegen::new()
        .with_options(codegen_options)
        .build(&program)
        .code;

      // Only use minified version if it's actually smaller
      if minified.len() < code.len() {
        out.extend_from_slice(minified.as_bytes());
        return true;
      }
    }
  }
  false
}

#[cfg(not(feature = "js"))]
fn minify_js_with_oxc(_cfg: &Cfg, _mode: TopLevelMode, _out: &mut Vec<u8>, _code: &[u8]) -> bool {
  false
}
