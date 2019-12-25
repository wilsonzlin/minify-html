use std::fs::File;
use std::io::{Read, stdin, stdout, Write};
use structopt::StructOpt;

use hyperbuild::hyperbuild;

#[derive(StructOpt)]
struct Cli {
    #[structopt(short, long, parse(from_os_str))]
    src: std::path::PathBuf,
    #[structopt(short, long, parse(from_os_str))]
    out: std::path::PathBuf,
}

fn main() {
    let args = Cli::from_args();
    let mut vec = Vec::<u8>::new();
    let mut src_file = File::open(args.src).expect("could not read source file");
    src_file.read_to_end(&mut vec);
    let mut code = vec.as_mut_slice();
    // TODO
    let result = hyperbuild(code).unwrap();
    println!("{}", result);
    let mut out_file = File::create(args.out).expect("could not open output file");
    out_file.write_all(&code[..result]).expect("could not write to output file");
    println!("Done!")
}
