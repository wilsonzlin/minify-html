use std::io::{stdin, stdout, Read};

use minify_html::canonicalise;

fn main() {
    let mut src = Vec::new();
    stdin().read_to_end(&mut src).unwrap();
    canonicalise(&mut stdout(), &src).unwrap();
}
