use std::fs::File;
use std::io::{stdin, stdout, Read, Write};
use std::process::exit;
use std::sync::Arc;

use rayon::iter::{IntoParallelRefIterator, ParallelIterator};
use structopt::StructOpt;

use minify_html::{minify, Cfg};

#[derive(StructOpt)]
#[structopt(
    name = "minify-html",
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

    /// Minify JS in `<script>` tags that have a valid or no `type` attribute value.
    #[structopt(long)]
    minify_js: bool,

    /// Minify CSS in `<style>` tags and `style` attributes.
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

    let cfg = Arc::new(Cfg {
        do_not_minify_doctype: args.do_not_minify_doctype,
        ensure_spec_compliant_unquoted_attribute_values: args
            .ensure_spec_compliant_unquoted_attribute_values,
        keep_closing_tags: args.keep_closing_tags,
        keep_comments: args.keep_comments,
        keep_html_and_head_opening_tags: args.keep_html_and_head_opening_tags,
        keep_spaces_between_attributes: args.keep_spaces_between_attributes,
        minify_css: args.minify_css,
        minify_js: args.minify_js,
        remove_bangs: args.remove_bangs,
        remove_processing_instructions: args.remove_processing_instructions,
    });

    if args.inputs.len() <= 1 {
        // single file mode or stdin mode
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

            let mut src_file =
                io_expect!(input_name, File::open(&input), "Could not open source file");
            let mut src_code = Vec::<u8>::new();
            io_expect!(
                input_name,
                src_file.read_to_end(&mut src_code),
                "Could not load source code"
            );
            let out_code = minify(&src_code, &cfg);
            let mut out_file = io_expect!(
                input_name,
                File::create(&input),
                "Could not open output file"
            );
            io_expect!(
                input_name,
                out_file.write_all(&out_code),
                "Could not save minified code"
            );
            println!("{}", input_name);
        });
    }
}
