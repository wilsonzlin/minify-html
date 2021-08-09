use std::io::stdout;
use std::time::Instant;
use std::{env, fs};

use minify_html::{minify, Cfg};

fn main() {
    let iterations = env::var("MHB_ITERATIONS")
        .unwrap()
        .parse::<usize>()
        .unwrap();
    let input_dir = env::var("MHB_INPUT_DIR").unwrap();
    let html_only = env::var("MHB_HTML_ONLY")
        .ok()
        .filter(|v| v == "1")
        .is_some();

    let tests = fs::read_dir(input_dir).unwrap().map(|d| d.unwrap());

    let mut results: Vec<(String, usize, usize, f64)> = Vec::new();
    let mut cfg = Cfg::new();
    if !html_only {
        cfg.minify_css = true;
        cfg.minify_js = true;
    };

    for t in tests {
        let source = fs::read(t.path()).unwrap();
        let start = Instant::now();
        let mut len = 0;
        for _ in 0..iterations {
            len = minify(&source, &cfg).len();
        }
        let elapsed = start.elapsed().as_secs_f64();
        results.push((
            t.file_name().into_string().unwrap(),
            len,
            iterations,
            elapsed,
        ));
    }

    serde_json::to_writer(stdout(), &results).unwrap();
}
