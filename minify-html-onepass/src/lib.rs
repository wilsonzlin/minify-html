pub use crate::cfg::Cfg;
use crate::common::spec::tag::ns::Namespace;
use crate::err::debug_repr;
pub use crate::err::{Error, ErrorType, FriendlyError};
use crate::proc::Processor;
use crate::unit::content::process_content;

mod cfg;
mod common;
mod err;
#[macro_use]
mod proc;
#[cfg(test)]
mod tests;
mod unit;

/// Minifies a slice in-place and returns the new minified length.
/// Any original code after the end of the minified code is left intact.
///
/// # Arguments
///
/// * `code` - A mutable slice of bytes representing the source code to minify.
/// * `cfg` - Configuration object to adjust minification approach.
///
/// # Examples
///
/// ```
/// use minify_html_onepass::{Cfg, Error, in_place};
///
/// let mut code = b"<p>  Hello, world!  </p>".to_vec();
/// let cfg = &Cfg {
///     minify_js: false,
///     minify_css: false,
/// };
/// match in_place(&mut code, cfg) {
///     Ok(minified_len) => assert_eq!(&code, b"<p>Hello, world!d!  </p>"),
///     Err(Error { error_type, position }) => {}
/// };
/// ```
pub fn in_place(code: &mut [u8], cfg: &Cfg) -> Result<usize, Error> {
    let mut proc = Processor::new(code);
    process_content(&mut proc, cfg, Namespace::Html, None, false)
        .and_then(|_| {
            if !proc.at_end() {
                Err(ErrorType::UnexpectedClosingTag)
            } else {
                Ok(())
            }
        })
        .map_err(|error_type| Error {
            error_type,
            position: proc.read_len(),
        })?;
    proc.finish()
}

/// Minifies a str in-place and returns the new minified str.
/// Any original code after the end of the minified code is left intact.
///
/// # Arguments
///
/// * `code` - A mutable str representing the source code to minify.
/// * `cfg` - Configuration object to adjust minification approach.
///
/// # Examples
///
/// ```
/// use minify_html_onepass::{Cfg, Error, in_place_str};
///
/// let mut code = "<p>  Hello, world!  </p>".to_string();
/// let cfg = &Cfg {
///     minify_js: false,
///     minify_css: false,
/// };
/// match in_place_str(&mut code, cfg) {
///     Ok(minified_len) => assert_eq!(&code, "<p>Hello, world!d!  </p>"),
///     Err(Error { error_type, position }) => {}
/// };
/// ```
pub fn in_place_str<'s>(code: &'s mut str, cfg: &Cfg) -> Result<&'s str, Error> {
    let bytes = unsafe { code.as_bytes_mut() };
    match in_place(bytes, cfg) {
        Ok(min_len) => Ok(unsafe { std::str::from_utf8_unchecked(&bytes[..min_len]) }),
        Err(e) => Err(e),
    }
}

/// Minifies a Vec in-place, truncating it to the minified length.
///
/// # Arguments
///
/// * `code` - A slice of bytes representing the source code to minify.
/// * `cfg` - Configuration object to adjust minification approach.
///
/// # Examples
///
/// ```
/// use minify_html_onepass::{Cfg, Error, truncate};
///
/// let mut code = b"<p>  Hello, world!  </p>".to_vec();
/// let cfg = &Cfg {
///     minify_js: false,
///     minify_css: false,
/// };
/// match truncate(&mut code, cfg) {
///     Ok(()) => assert_eq!(code, b"<p>Hello, world!".to_vec()),
///     Err(Error { error_type, position }) => {}
/// };
/// ```
pub fn truncate(code: &mut Vec<u8>, cfg: &Cfg) -> Result<(), Error> {
    match in_place(code, cfg) {
        Ok(written_len) => {
            code.truncate(written_len);
            Ok(())
        }
        Err(e) => Err(e),
    }
}

/// Copies a slice into a new Vec and minifies it, returning the Vec.
/// The resulting Vec will only contain minified code.
///
/// # Arguments
///
/// * `code` - A slice of bytes representing the source code to minify.
/// * `cfg` - Configuration object to adjust minification approach.
///
/// # Examples
///
/// ```
/// use minify_html_onepass::{Cfg, Error, copy};
///
/// let mut code: &[u8] = b"<p>  Hello, world!  </p>";
/// let cfg = &Cfg {
///     minify_js: false,
///     minify_css: false,
/// };
/// match copy(&code, cfg) {
///     Ok(minified) => {
///         assert_eq!(code, b"<p>  Hello, world!  </p>");
///         assert_eq!(minified, b"<p>Hello, world!".to_vec());
///     }
///     Err(Error { error_type, position }) => {}
/// };
/// ```
pub fn copy(code: &[u8], cfg: &Cfg) -> Result<Vec<u8>, Error> {
    let mut copy = code.to_vec();
    match truncate(&mut copy, cfg) {
        Ok(()) => Ok(copy),
        Err(e) => Err(e),
    }
}

/// Minifies a slice in-place and returns the new minified length.
/// Any original code after the end of the minified code is left intact.
///
/// This function is identical to `in_place` except it returns a `FriendlyError` on error instead.
///
/// `FriendlyError` has a `code_context` field, which is a string of a visual representation of the
/// source, with line numbers and position markers to aid in debugging syntax.
///
/// # Arguments
///
/// * `code` - A mutable slice of bytes representing the source code to minify.
/// * `cfg` - Configuration object to adjust minification approach.
///
/// # Examples
///
/// ```
/// use minify_html_onepass::{Cfg, FriendlyError, with_friendly_error};
///
/// let mut code = b"<p></div>".to_vec();
/// let cfg = &Cfg {
///     minify_js: false,
///     minify_css: false,
/// };
/// match with_friendly_error(&mut code, cfg) {
///     Ok(minified_len) => {}
///     Err(FriendlyError { position, message, code_context }) => {
///         assert_eq!(position, 3);
///         assert_eq!(message, "Unexpected closing tag.");
///         assert_eq!(code_context, concat!(
///             "1|<p></div>\n",
///             ">|   ^ \n",
///         ));
///     }
/// };
/// ```
pub fn with_friendly_error(code: &mut [u8], cfg: &Cfg) -> Result<usize, FriendlyError> {
    in_place(code, cfg).map_err(|err| FriendlyError {
        position: err.position,
        message: err.error_type.message(),
        code_context: debug_repr(code, err.position as isize, -1),
    })
}
