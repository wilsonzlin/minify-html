pub use crate::err::ErrorType as ErrorType;
use crate::proc::Processor;
use crate::unit::content::process_content;
use crate::unit::tag::Namespace;

mod err;
mod pattern;
#[macro_use]
mod proc;
mod spec;
mod unit;

pub fn hyperbuild(code: &mut [u8]) -> Result<usize, (ErrorType, usize)> {
    let mut proc = Processor::new(code);
    match process_content(&mut proc, Namespace::Html, None) {
        Ok(()) => Ok(proc.written_len()),
        Err(e) => Err((e, proc.read_len())),
    }
}
