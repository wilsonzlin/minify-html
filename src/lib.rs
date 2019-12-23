mod code;
mod err;
mod proc;
mod spec;

use err::HbRes;
use crate::code::Code;
use crate::proc::content::process_content;
use crate::proc::Processor;

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
fn hyperbuild<T: Code>(code: &mut T) -> HbRes<()> {
    process_content(&Processor { data: code }, None)
}
