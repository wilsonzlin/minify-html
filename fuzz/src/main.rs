use afl::fuzz;
use hyperbuild::{hyperbuild, ErrorType};

fn main() {
    fuzz!(|data: &[u8]| {
        let mut mut_data: Vec<u8> = data.iter().map(|x| *x).collect();
        hyperbuild(&mut mut_data);
    });
}
