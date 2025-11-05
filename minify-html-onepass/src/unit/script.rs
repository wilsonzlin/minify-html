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

/// Strips treeshake annotations like /*#__PURE__*/ and /*@__PURE__*/
/// These annotations are only useful for bundlers during tree-shaking,
/// and waste bytes in inline scripts where no further bundling occurs.
fn strip_treeshake_annotations(code: &str) -> String {
  let mut result = String::with_capacity(code.len());
  let bytes = code.as_bytes();
  let mut i = 0;

  while i < bytes.len() {
    // Check for comment start
    if i + 1 < bytes.len() && bytes[i] == b'/' && bytes[i + 1] == b'*' {
      // Find comment end
      let comment_start = i;
      i += 2;
      let mut comment_end = None;

      while i + 1 < bytes.len() {
        if bytes[i] == b'*' && bytes[i + 1] == b'/' {
          comment_end = Some(i + 2);
          break;
        }
        i += 1;
      }

      if let Some(end) = comment_end {
        // Extract comment content (between /* and */)
        let comment = &code[comment_start + 2..end - 2];
        let trimmed_comment = comment.trim();

        // Check if it's a treeshake annotation
        if trimmed_comment == "#__PURE__"
          || trimmed_comment == "@__PURE__"
          || trimmed_comment == "__PURE__" {
          // Skip this comment entirely
          i = end;
          continue;
        }

        // Not a treeshake annotation, keep the comment
        result.push_str(&code[comment_start..end]);
        i = end;
      } else {
        // Unterminated comment, keep it as-is
        result.push(bytes[i - 2] as char);
        i -= 1;
      }
    } else {
      result.push(bytes[i] as char);
      i += 1;
    }
  }

  result
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

        // Strip treeshake annotations (e.g., /*#__PURE__*/, /*@__PURE__*/)
        // These are only useful for bundlers, not inline scripts
        let minified = strip_treeshake_annotations(&minified);

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
