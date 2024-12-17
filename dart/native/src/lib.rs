use std::{cell::RefCell, ptr, slice};

use minify_html::Cfg;

thread_local! {
    static LAST_RESULT: RefCell<Option<Vec<u8>>> = RefCell::new(None);
}

#[no_mangle]
pub extern "C" fn minify_html(
    input: *const u8,
    length: usize,
    do_not_minify_doctype: bool,
    ensure_spec_compliant_unquoted_attribute_values: bool,
    keep_closing_tags: bool,
    keep_html_and_head_opening_tags: bool,
    keep_spaces_between_attributes: bool,
    keep_comments: bool,
    minify_css: bool,
    minify_css_level_1: bool,
    minify_css_level_2: bool,
    minify_css_level_3: bool,
    minify_js: bool,
    remove_bangs: bool,
    remove_processing_instructions: bool,
) -> usize {
    let src = unsafe { slice::from_raw_parts(input, length) };

    let cfg = Cfg {
        do_not_minify_doctype,
        ensure_spec_compliant_unquoted_attribute_values,
        keep_closing_tags,
        keep_html_and_head_opening_tags,
        keep_spaces_between_attributes,
        keep_comments,
        minify_css,
        minify_css_level_1,
        minify_css_level_2,
        minify_css_level_3,
        minify_js,
        remove_bangs,
        remove_processing_instructions,
    };

    let result = minify_html::minify(src, &cfg);
    let len = result.len();

    LAST_RESULT.with(|v| *v.borrow_mut() = Some(result));
    len
}

#[no_mangle]
pub extern "C" fn get_last_result() -> *const u8 {
    LAST_RESULT.with(|prev| match prev.borrow().as_ref() {
        Some(bytes) => bytes.as_ptr(),
        None => return ptr::null(),
    })
}

#[no_mangle]
pub extern "C" fn clear_last_result() {
    LAST_RESULT.with(|value| *value.borrow_mut() = None)
}
