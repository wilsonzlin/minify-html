use crate::cfg::Cfg;
use crate::err::ProcessingResult;
use crate::proc::MatchAction::*;
use crate::proc::MatchMode::*;
use crate::proc::Processor;
use aho_corasick::AhoCorasick;
use aho_corasick::AhoCorasickBuilder;
use once_cell::sync::Lazy;
use oxc_allocator::Allocator;
use oxc_codegen::Codegen;
use oxc_codegen::CodegenOptions;
use oxc_minifier::CompressOptions;
use oxc_minifier::MangleOptions;
use oxc_minifier::Minifier;
use oxc_minifier::MinifierOptions;
use oxc_parser::Parser;
use oxc_span::SourceType;

static SCRIPT_END: Lazy<AhoCorasick> = Lazy::new(|| {
  AhoCorasickBuilder::new()
    .ascii_case_insensitive(true)
    .build(["</script"])
    .unwrap()
});

/// Represents the mode in which JavaScript should be parsed and minified
#[derive(Debug, Clone, Copy)]
pub enum TopLevelMode {
  /// Parse as a global script
  Global,
  /// Parse as an ES module
  Module,
}

// Provide `None` to `mode` if not JS.
#[inline(always)]
pub fn process_script(
  proc: &mut Processor,
  cfg: &Cfg,
  mode: Option<TopLevelMode>,
) -> ProcessingResult<()> {
  proc.require_not_at_end()?;
  let src = proc.m(WhileNotSeq(&SCRIPT_END), Discard);
  // `process_tag` will require closing tag.

  if cfg.minify_js && mode.is_some() {
    let code = &proc[src];
    // Try to convert bytes to UTF-8 string for parsing
    if let Ok(source_text) = std::str::from_utf8(code) {
      let allocator = Allocator::default();

      // Determine source type based on mode
      let source_type = match mode.unwrap() {
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
        let minifier_options = MinifierOptions {
          mangle: Some(MangleOptions::default()),
          compress: Some(CompressOptions::safest()),
        };
        let _minifier_ret = Minifier::new(minifier_options).minify(&allocator, &mut program);

        // Generate minified code
        let codegen_options = CodegenOptions {
          minify: true,
          ..CodegenOptions::default()
        };
        let minified = Codegen::new()
          .with_options(codegen_options)
          .build(&program)
          .code;

        // Only use minified version if it's actually smaller
        if minified.len() < src.len() {
          proc.write_slice(minified.as_bytes());
          return Ok(());
        }
      }
    }
    // Fall back to original code
    proc.write_range(src);
  } else {
    proc.write_range(src);
  };

  Ok(())
}
