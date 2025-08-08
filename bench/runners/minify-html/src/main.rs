use minify_html::minify;
use minify_html::Cfg;
use std::env;
use std::fs;
use std::io::stdout;
use std::time::Instant;

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
  let mut cfg = Cfg::new();
  cfg.enable_possibly_noncompliant();
  if !html_only {
    #[cfg(feature = "lightningcss")]
    {
      cfg.minify_css = true;
    }
    #[cfg(feature = "minify-js")]
    {
      cfg.minify_js = true;
    }
  };

  for t in fs::read_dir(input_dir).unwrap().map(|d| d.unwrap()) {
    let source = fs::read(t.path()).unwrap();
    let input_name = t.file_name().into_string().unwrap();

    let output = minify(&source, &cfg);
    let len = output.len();
    if let Some(output_dir) = &output_dir {
      fs::write(format!("{}/{}", output_dir, input_name), output).unwrap();
    };

    let start = Instant::now();
    for _ in 0..iterations {
      let _ = minify(&source, &cfg);
    }
    let elapsed = start.elapsed().as_secs_f64();

    results.push((input_name, len, iterations, elapsed));
  }

  serde_json::to_writer(stdout(), &results).unwrap();
}
