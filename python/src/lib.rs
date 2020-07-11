use hyperbuild::{Cfg, hyperbuild as hyperbuild_native};
use pyo3::prelude::*;
use pyo3::exceptions::SyntaxError;
use pyo3::wrap_pyfunction;
use std::str::from_utf8_unchecked;

#[pyfunction(py_args="*", minify_js="false")]
fn minify(code: String, minify_js: bool) -> PyResult<String> {
    let mut code = code.into_bytes();
    match hyperbuild_native(&mut code, &Cfg {
        minify_js,
    }) {
        Ok(out_len) => Ok(unsafe { from_utf8_unchecked(&code[0..out_len]).to_string() }),
        Err((err, pos)) => Err(SyntaxError::py_err(format!("{} [Character {}]", err.message(), pos))),
    }
}

#[pymodule]
fn hyperbuild(_py: Python, m: &PyModule) -> PyResult<()> {
    m.add_wrapped(wrap_pyfunction!(minify))?;

    Ok(())
}
