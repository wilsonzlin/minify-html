use afl::fuzz;
use minify_html::{Cfg, in_place};

fn main() {
    fuzz!(|data: &[u8]| {
        let mut mut_data: Vec<u8> = data.iter().map(|x| *x).collect();
        let _ = in_place(&mut mut_data, &Cfg {
            minify_js: false,
            minify_css: false,
        });
    });
}
