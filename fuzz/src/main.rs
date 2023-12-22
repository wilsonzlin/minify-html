use afl::fuzz;
use minify_html_onepass::in_place;
use minify_html_onepass::Cfg;

fn main() {
  fuzz!(|data: &[u8]| {
    let mut mut_data = data.to_vec();
    let _ = in_place(&mut mut_data, &Cfg::new());
  });
}
