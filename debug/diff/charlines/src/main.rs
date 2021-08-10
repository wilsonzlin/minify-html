use std::io::{stdin, stdout, Read, Write};

fn main() {
    let mut src = Vec::new();
    stdin().read_to_end(&mut src).unwrap();
    let mut out = stdout();
    for c in src {
        out.write_all(&[c, b'\n']).unwrap();
    }
}
