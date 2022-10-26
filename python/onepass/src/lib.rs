use ::minify_html_onepass::{in_place as minify_html_native, Cfg, Error};
use pyo3::exceptions::PySyntaxError;
use pyo3::prelude::*;
use pyo3::wrap_pyfunction;
use std::str::from_utf8_unchecked;

#[pyfunction(py_args = "*", minify_js = "false", minify_css = "false")]
fn minify(code: String, minify_js: bool, minify_css: bool) -> PyResult<String> {
    let mut code = code.into_bytes();
    match minify_html_native(
        &mut code,
        &Cfg {
            minify_js,
            minify_css,
        },
    ) {
        Ok(out_len) => Ok(unsafe { from_utf8_unchecked(&code[0..out_len]).to_string() }),
        Err(Error {
            error_type,
            position,
        }) => Err(PySyntaxError::new_err(format!(
            "{} [Character {}]",
            error_type.message(),
            position
        ))),
    }
}

#[pymodule]
fn minify_html_onepass(_py: Python, m: &PyModule) -> PyResult<()> {
    m.add_wrapped(wrap_pyfunction!(minify))?;

    Ok(())
}
