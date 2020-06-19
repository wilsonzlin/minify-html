pub use crate::err::ErrorType as ErrorType;
use crate::proc::Processor;
use crate::unit::content::process_content;
use crate::spec::tag::ns::Namespace;

mod err;
mod gen;
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

pub fn hyperbuild_truncate(code: &mut Vec<u8>) -> Result<(), (ErrorType, usize)> {
    match hyperbuild(code) {
        Ok(written_len) => {
            code.truncate(written_len);
            Ok(())
        },
        Err(e) => Err(e),
    }
}

pub fn hyperbuild_copy(code: &[u8]) -> Result<Vec<u8>, (ErrorType, usize)> {
    let mut copy = code.to_vec();
    match hyperbuild_truncate(&mut copy) {
        Ok(()) => Ok(copy),
        Err(e) => Err(e),
    }
}

pub struct FriendlyError {
    // Make public to allow destructuring.
    pub position: usize,
    pub message: String,
    pub code_context: String,
}

pub fn hyperbuild_friendly_error(code: &mut [u8]) -> Result<usize, FriendlyError> {
    let mut proc = Processor::new(code);
    match process_content(&mut proc, Namespace::Html, None) {
        Ok(()) => Ok(proc.written_len()),
        Err(e) => Err(FriendlyError {
            position: proc.read_len(),
            message: e.message(),
            code_context: format!("{:?}", proc),
        }),
    }
}
