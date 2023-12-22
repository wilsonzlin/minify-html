use std::fs;
use std::io::Write;
use std::io::{self};
use std::panic;

fn main() {
  for dirent in fs::read_dir("../out/crashes").unwrap() {
    let path = dirent.unwrap().path().to_path_buf();
    let path_in_catch = path.clone();
    let res = panic::catch_unwind(|| {
      let mut contents = fs::read(path_in_catch).unwrap();
      let _ = minify_html_onepass::in_place(&mut contents, &minify_html_onepass::Cfg {
        minify_js: false,
        minify_css: false,
      });
    });
    if res.is_err() {
      let contents = fs::read(path).unwrap();
      io::stdout().write_all(&contents).unwrap();
      break;
    };
    fs::remove_file(path).unwrap();
  }
}
