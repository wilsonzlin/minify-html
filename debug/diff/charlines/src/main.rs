use std::io::stdin;
use std::io::stdout;
use std::io::Read;
use std::io::Write;

fn main() {
  let mut src = Vec::new();
  stdin().read_to_end(&mut src).unwrap();
  let mut out = stdout();
  for c in src {
    out.write_all(&[c, b'\n']).unwrap();
  }
}
