use std::fs::File;
use std::io::{Read, Write};

use structopt::StructOpt;

use hyperbuild::{hyperbuild_friendly, FriendlyError};

#[derive(StructOpt)]
struct Cli {
    #[structopt(short, long, parse(from_os_str))]
    src: std::path::PathBuf,
    #[structopt(short, long, parse(from_os_str))]
    out: std::path::PathBuf,
}

fn main() {
    let args = Cli::from_args();
    let mut code = Vec::<u8>::new();
    let mut src_file = File::open(args.src).expect("could not open source file");
    src_file.read_to_end(&mut code).expect("could not read source file");
    match hyperbuild_friendly(&mut code) {
        Ok(out_len) => {
            let mut out_file = File::create(args.out).expect("could not open output file");
            out_file.write_all(&code[..out_len]).expect("could not write to output file");
        }
        Err(FriendlyError { position, message, code_context }) => {
            eprintln!("Failed at character {}:", position);
            eprintln!("{}", message);
            eprintln!("The output file has not been touched.");
            eprintln!("-----");
            eprintln!("{}", code_context);
        }
    };
}
