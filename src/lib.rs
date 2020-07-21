pub use crate::err::{Error, ErrorType, FriendlyError};
use crate::proc::Processor;
use crate::unit::content::process_content;
use crate::spec::tag::ns::Namespace;
pub use crate::cfg::Cfg;

mod cfg;
mod err;
mod gen;
mod pattern;
#[macro_use]
mod proc;
mod spec;
mod tests;
mod unit;

pub fn in_place(code: &mut [u8], cfg: &Cfg) -> Result<usize, Error> {
    let mut proc = Processor::new(code);
    process_content(&mut proc, cfg, Namespace::Html, None)
        .map_err(|error_type| Error {
            error_type,
            position: proc.read_len(),
        })?;
    proc.finish()
}

pub fn in_place_str<'s>(code: &'s mut str, cfg: &Cfg) -> Result<&'s str, Error> {
    let bytes = unsafe { code.as_bytes_mut() };
    match in_place(bytes, cfg) {
        Ok(min_len) => Ok(unsafe { std::str::from_utf8_unchecked(&bytes[..min_len]) }),
        Err(e) => Err(e),
    }
}

pub fn truncate(code: &mut Vec<u8>, cfg: &Cfg) -> Result<(), Error> {
    match in_place(code, cfg) {
        Ok(written_len) => {
            code.truncate(written_len);
            Ok(())
        }
        Err(e) => Err(e),
    }
}

pub fn copy(code: &[u8], cfg: &Cfg) -> Result<Vec<u8>, Error> {
    let mut copy = code.to_vec();
    match truncate(&mut copy, cfg) {
        Ok(()) => Ok(copy),
        Err(e) => Err(e),
    }
}
