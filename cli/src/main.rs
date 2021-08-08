use std::fs::File;
use std::io::{stdin, stdout, Read, Write};

use structopt::StructOpt;

use minify_html::{minify, Cfg};

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
    /// Minify JS in `<script>` tags that have a valid or no `type` attribute value.
    #[structopt(long)]
    minify_js: bool,
    /// Minify CSS in `<style>` tags and `style` attributes.
    #[structopt(long)]
    minify_css: bool,
    /// Ensure all unquoted attribute values in the output do not contain any characters prohibited by the WHATWG specification.
    #[structopt(long)]
    ensure_spec_compliant_unquoted_attribute_values: bool,
    /// Do not omit closing tags when possible.
    #[structopt(long)]
    keep_closing_tags: bool,
    /// Do not omit `<html>` and `<head>` opening tags when they don't have attributes.
    #[structopt(long)]
    keep_html_and_head_opening_tags: bool,
    /// Keep spaces between attributes when possible to conform to HTML standards.
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
    let out_code = minify(
        &src_code,
        &Cfg {
            ensure_spec_compliant_unquoted_attribute_values: args.ensure_spec_compliant_unquoted_attribute_values,
            keep_closing_tags: args.keep_closing_tags,
            keep_comments: args.keep_comments,
            keep_html_and_head_opening_tags: args.keep_html_and_head_opening_tags,
            keep_spaces_between_attributes: args.keep_spaces_between_attributes,
            minify_css: args.minify_css,
            minify_js: args.minify_js,
            remove_bangs: args.remove_bangs,
            remove_processing_instructions: args.remove_processing_instructions,
        },
    );
    let mut out_file: Box<dyn Write> = match args.output {
        Some(p) => Box::new(io_expect!(File::create(p), "could not open output file")),
        None => Box::new(stdout()),
    };
    io_expect!(
        out_file.write_all(&out_code),
        "could not save minified code"
    );
}
