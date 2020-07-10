use afl::fuzz;
use hyperbuild::{Cfg, hyperbuild};

fn main() {
    fuzz!(|data: &[u8]| {
        let mut mut_data: Vec<u8> = data.iter().map(|x| *x).collect();
        let _ = hyperbuild(&mut mut_data, &Cfg {
            minify_js: false,
        });
    });
}
