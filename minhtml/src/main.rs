use minify_html::minify;
use minify_html::Cfg;
use rayon::iter::IntoParallelRefIterator;
use rayon::iter::ParallelIterator;
use std::fs::File;
use std::io::stdin;
use std::io::stdout;
use std::io::Read;
use std::io::Write;
use std::process::exit;
use std::sync::Arc;
use structopt::StructOpt;

#[derive(StructOpt)]
#[structopt(
  name = "minhtml",
  about = "Extremely fast and smart HTML + JS + CSS minifier"
)]
// WARNING: Keep descriptions in sync with Cfg.
struct Cli {
  /// Files to minify; omit for stdin. If more than one is provided, they will be parallel minified in place, and --output must be omitted.
  #[structopt(parse(from_os_str))]
  inputs: Vec<std::path::PathBuf>,

  /// Output destination; omit for stdout.
  #[structopt(short, long, parse(from_os_str))]
  output: Option<std::path::PathBuf>,

  /// Allow unquoted attribute values in the output to contain characters prohibited by the [WHATWG specification](https://html.spec.whatwg.org/multipage/syntax.html#attributes-2). These will still be parsed correctly by almost all browsers.
  #[structopt(long)]
  allow_noncompliant_unquoted_attribute_values: bool,

  /// Allow removing_spaces between attributes when possible, which may not be spec compliant. These will still be parsed correctly by almost all browsers.
  #[structopt(long)]
  allow_removing_spaces_between_attributes: bool,

  /// Do not omit closing tags when possible.
  #[structopt(long)]
  keep_closing_tags: bool,

  /// Keep all comments.
  #[structopt(long)]
  keep_comments: bool,

  /// Do not omit `<html>` and `<head>` opening tags when they don't have attributes.
  #[structopt(long)]
  keep_html_and_head_opening_tags: bool,

  /// Keep `type=text` attribute name and value on `<input>` elements.
  #[structopt(long)]
  keep_input_type_text_attr: bool,

  /// Keep SSI comments.
  #[structopt(long)]
  keep_ssi_comments: bool,

  /// Minify CSS in `<style>` tags and `style` attributes using [https://github.com/parcel-bundler/lightningcss](lightningcss).
  #[structopt(long)]
  minify_css: bool,

  /// Minify DOCTYPEs. Minified DOCTYPEs may not be spec compliant, but will still be parsed correctly by almost all browsers.
  #[structopt(long)]
  minify_doctype: bool,

  /// Minify JavaScript in `<script>` tags using
  /// [minify-js](https://github.com/wilsonzlin/minify-js).
  ///
  /// Only `<script>` tags with a valid or no
  /// [MIME type](https://mimesniff.spec.whatwg.org/#javascript-mime-type) is considered to
  /// contain JavaScript, as per the specification.
  #[structopt(long)]
  minify_js: bool,

  /// When `{{`, `{#`, or `{%` are seen in content, all source code until the subsequent matching closing `}}`, `#}`, or `%}` respectively gets piped through untouched.
  #[structopt(long)]
  preserve_brace_template_syntax: bool,

  /// When `<%` is seen in content, all source code until the subsequent matching closing `%>` gets piped through untouched.
  #[structopt(long)]
  preserve_chevron_percent_template_syntax: bool,

  /// Remove all bangs.
  #[structopt(long)]
  remove_bangs: bool,

  /// Remove all processing instructions.
  #[structopt(long)]
  remove_processing_instructions: bool,
}

macro_rules! io_expect {
  ($name:expr, $expr:expr, $msg:literal) => {
    match $expr {
      Ok(r) => r,
      Err(e) => {
        eprintln!("[{}] {}: {}", $name, $msg, e);
        return;
      }
    }
  };
}

fn main() {
  let args = Cli::from_args();
  if args.output.is_some() && args.inputs.len() > 1 {
    eprintln!("Cannot provide --output when multiple inputs are provided.");
    exit(1);
  };

  #[rustfmt::skip]
  let cfg = Arc::new(Cfg {
    allow_noncompliant_unquoted_attribute_values: args.allow_noncompliant_unquoted_attribute_values,
    allow_removing_spaces_between_attributes: args.allow_removing_spaces_between_attributes,
    keep_closing_tags: args.keep_closing_tags,
    keep_comments: args.keep_comments,
    keep_html_and_head_opening_tags: args.keep_html_and_head_opening_tags,
    keep_input_type_text_attr: args.keep_input_type_text_attr,
    keep_ssi_comments: args.keep_ssi_comments,
    minify_css: args.minify_css,
    minify_doctype: args.minify_doctype,
    minify_js: args.minify_js,
    preserve_brace_template_syntax: args.preserve_brace_template_syntax,
    preserve_chevron_percent_template_syntax: args.preserve_chevron_percent_template_syntax,
    remove_bangs: args.remove_bangs,
    remove_processing_instructions: args.remove_processing_instructions,
  });

  if args.inputs.len() <= 1 {
    // Single file mode or stdin mode.
    let input_name = args
      .inputs
      .get(0)
      .map(|p| p.to_string_lossy().into_owned())
      .unwrap_or_else(|| "stdin".to_string());
    let mut src_file: Box<dyn Read> = match args.inputs.get(0) {
      Some(p) => Box::new(io_expect!(
        input_name,
        File::open(p),
        "Could not open source file"
      )),
      None => Box::new(stdin()),
    };
    let mut src_code = Vec::<u8>::new();
    io_expect!(
      input_name,
      src_file.read_to_end(&mut src_code),
      "Could not load source code"
    );
    let out_code = minify(&src_code, &cfg);
    let mut out_file: Box<dyn Write> = match args.output {
      Some(p) => Box::new(io_expect!(
        input_name,
        File::create(p),
        "Could not open output file"
      )),
      None => Box::new(stdout()),
    };
    io_expect!(
      input_name,
      out_file.write_all(&out_code),
      "Could not save minified code"
    );
  } else {
    args.inputs.par_iter().for_each(|input| {
      let input_name = input.to_string_lossy().into_owned();

      let mut src_file = io_expect!(input_name, File::open(input), "Could not open source file");
      let mut src_code = Vec::<u8>::new();
      io_expect!(
        input_name,
        src_file.read_to_end(&mut src_code),
        "Could not load source code"
      );
      let out_code = minify(&src_code, &cfg);
      let mut out_file = io_expect!(
        input_name,
        File::create(input),
        "Could not open output file"
      );
      io_expect!(
        input_name,
        out_file.write_all(&out_code),
        "Could not save minified code"
      );
      // Just print the name, since this is the default output and any prefix becomes redundant. It'd also allow piping into another command (quite nice for something like `minify-html *.html | xargs gzip`), copying as list of files, etc.
      println!("{}", input_name);
    });
  }
}
