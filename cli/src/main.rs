use std::fmt;
use std::fs::File;
use std::io::{stdin, stdout, Read, Write};
use std::str::FromStr;

use structopt::StructOpt;

use minify_html::{minify, minify_css, minify_js, Cfg};

#[derive(PartialEq)]
enum SourceType {
    Html,
    Css,
    Js,
}

impl Default for SourceType {
    fn default() -> Self {
        SourceType::Html
    }
}

impl fmt::Display for SourceType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "{}",
            match self {
                SourceType::Html => "html",
                SourceType::Css => "css",
                SourceType::Js => "js",
            }
        )
    }
}

impl FromStr for SourceType {
    type Err = String;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        match s.to_ascii_lowercase().as_str() {
            "html" => Ok(SourceType::Html),
            "css" => Ok(SourceType::Css),
            "js" => Ok(SourceType::Js),
            _ => Err(format!("\"{}\" is not \"html\", \"css\" or \"js\".", s)),
        }
    }
}

#[derive(StructOpt)]
#[structopt(
    name = "minify-html",
    about = "Extremely fast and smart HTML + JS + CSS minifier"
)]
// WARNING: Keep descriptions in sync with Cfg.
struct Cli {
    /// File to minify; omit for stdin.
    #[structopt(parse(from_os_str))]
    input: Option<std::path::PathBuf>,

    /// Output destination; omit for stdout.
    #[structopt(short, long, parse(from_os_str))]
    output: Option<std::path::PathBuf>,

    /// Minify Js in `<script>` tags that have a valid or no `type` attribute value.
    #[structopt(long)]
    minify_js: bool,

    /// Minify Css in `<style>` tags and `style` attributes.
    #[structopt(long)]
    minify_css: bool,

    #[structopt(long)]
    /// Do not minify DOCTYPEs. Minified DOCTYPEs may not be spec compliant.
    do_not_minify_doctype: bool,

    /// Ensure all unquoted attribute values in the output do not contain any characters prohibited by the WHATWG specification.
    #[structopt(long)]
    ensure_spec_compliant_unquoted_attribute_values: bool,

    /// Do not omit closing tags when possible.
    #[structopt(long)]
    keep_closing_tags: bool,

    /// Do not omit `<html>` and `<head>` opening tags when they don't have attributes.
    #[structopt(long)]
    keep_html_and_head_opening_tags: bool,

    /// Keep spaces between attributes when possible to conform to Html standards.
    #[structopt(long)]
    keep_spaces_between_attributes: bool,

    /// Keep all comments.
    #[structopt(long)]
    keep_comments: bool,

    /// Remove all bangs.
    #[structopt(long)]
    remove_bangs: bool,

    /// Remove all processing_instructions.
    #[structopt(long)]
    remove_processing_instructions: bool,

    /// Allowed values: "html", "css" or "js" (not case-sensitive).
    #[structopt(default_value, long)]
    source_type: SourceType,
}

macro_rules! io_expect {
    ($expr:expr, $msg:literal) => {
        match $expr {
            Ok(r) => r,
            Err(e) => {
                eprintln!("Error: {}", $msg);
                eprintln!("{}", e);
                return;
            }
        }
    };
}

fn main() {
    let args = Cli::from_args();
    let mut src_code = Vec::<u8>::new();
    let mut src_file: Box<dyn Read> = match args.input {
        Some(p) => Box::new(io_expect!(File::open(p), "could not open source file")),
        None => Box::new(stdin()),
    };
    io_expect!(
        src_file.read_to_end(&mut src_code),
        "could not load source code"
    );
    let cfg = &Cfg {
        do_not_minify_doctype: args.do_not_minify_doctype,
        ensure_spec_compliant_unquoted_attribute_values: args
            .ensure_spec_compliant_unquoted_attribute_values,
        keep_closing_tags: args.keep_closing_tags,
        keep_comments: args.keep_comments,
        keep_html_and_head_opening_tags: args.keep_html_and_head_opening_tags,
        keep_spaces_between_attributes: args.keep_spaces_between_attributes,
        minify_css: args.minify_css | (args.source_type == SourceType::Css),
        minify_js: args.minify_js | (args.source_type == SourceType::Js),
        remove_bangs: args.remove_bangs,
        remove_processing_instructions: args.remove_processing_instructions,
    };
    let out_code = match args.source_type {
        SourceType::Html => minify(&src_code, cfg),
        SourceType::Css => minify_css(&src_code, cfg),
        SourceType::Js => minify_js(&src_code, cfg),
    };
    let mut out_file: Box<dyn Write> = match args.output {
        Some(p) => Box::new(io_expect!(File::create(p), "could not open output file")),
        None => Box::new(stdout()),
    };
    io_expect!(
        out_file.write_all(&out_code),
        "could not save minified code"
    );
}
