use std::fs::File;
use std::io::{Read, stdin, stdout, Write};

use structopt::StructOpt;

use minify_html::{Cfg, FriendlyError, with_friendly_error};

#[derive(StructOpt)]
#[structopt(name = "minify-html", about = "Extremely fast and smart HTML + JS + CSS minifier")]
struct Cli {
    /// File to minify; omit for stdin.
    #[structopt(short, long, parse(from_os_str))]
    src: Option<std::path::PathBuf>,
    /// Output destination; omit for stdout.
    #[structopt(short, long, parse(from_os_str))]
    out: Option<std::path::PathBuf>,
    /// Enables JS minification.
    #[structopt(long)]
    js: bool,
    /// Enables CSS minification.
    #[structopt(long)]
    css: bool,
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
    let mut code = Vec::<u8>::new();
    let mut src_file: Box<dyn Read> = match args.src {
        Some(p) => Box::new(io_expect!(File::open(p), "could not open source file")),
        None => Box::new(stdin()),
    };
    io_expect!(src_file.read_to_end(&mut code), "could not load source code");
    match with_friendly_error(&mut code, &Cfg {
        minify_js: args.js,
        minify_css: args.css,
    }) {
        Ok(out_len) => {
            let mut out_file: Box<dyn Write> = match args.out {
                Some(p) => Box::new(io_expect!(File::create(p), "could not open output file")),
                None => Box::new(stdout()),
            };
            io_expect!(out_file.write_all(&code[..out_len]), "could not save minified code");
        }
        Err(FriendlyError { position, message, code_context }) => {
            eprintln!("Failed at character {}:", position);
            eprintln!("{}", message);
            if args.out.is_some() {
                eprintln!("The output file has not been touched.");
            };
            eprintln!("--------");
            eprintln!("{}", code_context);
        }
    };
}
