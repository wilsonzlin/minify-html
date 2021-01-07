use minify_html::{Cfg, in_place};
use std::fs;
use std::io::{stdout};
use std::time::Instant;
use structopt::StructOpt;

#[derive(StructOpt)]
struct Args {
    #[structopt(long, parse(from_os_str))]
    tests: std::path::PathBuf,
    #[structopt(long)]
    iterations: usize,
}

fn main() {
    let args = Args::from_args();
    let tests = fs::read_dir(args.tests).unwrap().map(|d| d.unwrap());

    let mut results: Vec<(String, f64)> = Vec::new();

    for t in tests {
        let source = fs::read(t.path()).unwrap();
        let start = Instant::now();
        for _ in 0..args.iterations {
            let mut data = source.to_vec();
            in_place(&mut data, &Cfg {
                minify_js: false,
                minify_css: false,
            }).unwrap();
        };
        let elapsed = start.elapsed().as_secs_f64();
        let ops = args.iterations as f64 / elapsed;
        results.push((t.file_name().to_str().unwrap().to_string(), ops));
    };

    serde_json::to_writer(stdout(), &results).unwrap();
}
