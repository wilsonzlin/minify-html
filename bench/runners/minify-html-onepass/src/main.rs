use std::io::stdout;
use std::time::Instant;
use std::{env, fs};

use minify_html_onepass::{in_place, Cfg};

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
    let output_dir = env::var("MHB_OUTPUT_DIR").ok();

    let mut results: Vec<(String, usize, usize, f64)> = Vec::new();
    let cfg = Cfg {
        minify_css: !html_only,
        minify_js: !html_only,
    };

    for t in fs::read_dir(input_dir).unwrap().map(|d| d.unwrap()) {
        let source = fs::read(t.path()).unwrap();
        let input_name = t.file_name().into_string().unwrap();

        let mut output = source.to_vec();
        let len = in_place(&mut output, &cfg).expect("failed to minify");
        output.truncate(len);
        if let Some(output_dir) = &output_dir {
            fs::write(format!("{}/{}", output_dir, input_name), output).unwrap();
        };

        let start = Instant::now();
        for _ in 0..iterations {
            let mut data = source.to_vec();
            let _ = in_place(&mut data, &cfg).expect("failed to minify");
        }
        let elapsed = start.elapsed().as_secs_f64();

        results.push((input_name, len, iterations, elapsed));
    }

    serde_json::to_writer(stdout(), &results).unwrap();
}
