use ::minify_html::{minify as minify_html_native, Cfg};
use pyo3::prelude::*;
use pyo3::wrap_pyfunction;
use std::string::String;

#[pyfunction(
    py_args = "*",
    do_not_minify_doctype = "false",
    ensure_spec_compliant_unquoted_attribute_values = "false",
    keep_closing_tags = "false",
    keep_comments = "false",
    keep_html_and_head_opening_tags = "false",
    keep_spaces_between_attributes = "false",
    minify_css = "false",
    minify_js = "false",
    remove_bangs = "false",
    remove_processing_instructions = "false"
)]
fn minify(
    code: String,
    do_not_minify_doctype: bool,
    ensure_spec_compliant_unquoted_attribute_values: bool,
    keep_closing_tags: bool,
    keep_comments: bool,
    keep_html_and_head_opening_tags: bool,
    keep_spaces_between_attributes: bool,
    minify_css: bool,
    minify_js: bool,
    remove_bangs: bool,
    remove_processing_instructions: bool,
) -> PyResult<String> {
    let code = code.into_bytes();
    let out_code = minify_html_native(
        &code,
        &Cfg {
            do_not_minify_doctype,
            ensure_spec_compliant_unquoted_attribute_values,
            keep_closing_tags,
            keep_comments,
            keep_html_and_head_opening_tags,
            keep_spaces_between_attributes,
            minify_css,
            minify_js,
            remove_bangs,
            remove_processing_instructions,
        },
    );
    Ok(String::from_utf8(out_code).unwrap())
}

#[pymodule]
fn minify_html(_py: Python, m: &PyModule) -> PyResult<()> {
    m.add_wrapped(wrap_pyfunction!(minify))?;

    Ok(())
}
