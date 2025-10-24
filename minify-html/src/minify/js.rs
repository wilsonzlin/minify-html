use crate::cfg::Cfg;
use minify_html_common::whitespace::trimmed;
use oxc_allocator::Allocator;
use oxc_codegen::Codegen;
use oxc_codegen::CodegenOptions;
use oxc_minifier::CompressOptions;
use oxc_minifier::MangleOptions;
use oxc_minifier::Minifier;
use oxc_minifier::MinifierOptions;
use oxc_parser::Parser;
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
  if cfg.minify_js {
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
        let codegen_options = CodegenOptions {
          minify: true,
          ..CodegenOptions::default()
        };
        let minified = Codegen::new()
          .with_options(codegen_options)
          .build(&program)
          .code;

        // Only use minified version if it's actually smaller
        if minified.len() < code.len() {
          out.extend_from_slice(minified.as_bytes());
          return;
        }
      }
    }
  }
  // Fall back to trimmed original code
  out.extend_from_slice(trimmed(code));
}
