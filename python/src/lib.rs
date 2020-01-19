use hyperbuild::hyperbuild as hyperbuild_native;
use pyo3::prelude::*;
use pyo3::exceptions::SyntaxError;
use pyo3::wrap_pyfunction;
use std::str::from_utf8_unchecked;

#[pyfunction]
fn minify(code: String) -> PyResult<String> {
    let mut code = code.into_bytes();
    match hyperbuild_native(&mut code) {
        Ok(out_len) => Ok(unsafe { from_utf8_unchecked(&code[0..out_len]).to_string() }),
        Err((err, pos)) => Err(SyntaxError::py_err(format!("{} [Character {}]", err.message(), pos))),
    }
}

#[pymodule]
fn hyperbuild(py: Python, m: &PyModule) -> PyResult<()> {
    m.add_wrapped(wrap_pyfunction!(minify))?;

    Ok(())
}
