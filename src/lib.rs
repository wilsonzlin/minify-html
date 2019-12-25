use crate::err::{ErrorType};
use crate::proc::Processor;
use crate::unit::content::process_content;

pub mod err;
#[macro_use]
mod proc;
mod spec;
mod unit;

/**
 * Run hyperbuild on an input array and write to {@param output}. Output will be
 * null terminated if no error occurs. WARNING: Input must end with '\xFF' or
 * '\0', and {@param input_size} must not include the terminator. WARNING: Does
 * not check if {@param output} is large enough. It should at least match the
 * size of the input.
 *
 * @param input input array to process
 * @param output output array to write to
 * @param cfg configuration to use
 * @return result where to write any resulting error information
 */
pub fn hyperbuild(code: &mut [u8]) -> Result<usize, (ErrorType, usize)> {
    let mut p = Processor::new(code);
    match process_content(&mut p, None) {
        Ok(()) => Ok(p.written_len()),
        Err(e) => Err((e, p.read_len())),
    }
}
