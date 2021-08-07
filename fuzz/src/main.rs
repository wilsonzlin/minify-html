use afl::fuzz;
use minify_html::{minify, Cfg};

fn main() {
    fuzz!(|data: &[u8]| {
        let mut_data: Vec<u8> = data.iter().copied().collect();
        let _ = minify(&mut_data, &Cfg::new());
    });
}
