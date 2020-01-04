use afl::fuzz;
use hyperbuild::hyperbuild;

fn main() {
    fuzz!(|data: &[u8]| {
        let mut mut_data: Vec<u8> = data.iter().map(|x| *x).collect();
        let _ = hyperbuild(&mut mut_data);
    });
}
