use minify_html::{minify, Cfg};
use std::slice;

#[no_mangle]
pub extern "C" fn ffi_create_cfg(
    keep_closing_tags: bool,
    keep_comments: bool,
    keep_html_and_head_opening_tags: bool,
    keep_spaces_between_attributes: bool,
    minify_css: bool,
    minify_js: bool,
    remove_bangs: bool,
    remove_processing_instructions: bool,
) -> *const Cfg {
    Box::into_raw(Box::new(Cfg {
        keep_closing_tags,
        keep_comments,
        keep_html_and_head_opening_tags,
        keep_spaces_between_attributes,
        minify_css,
        minify_js,
        remove_bangs,
        remove_processing_instructions,
    }))
}

#[no_mangle]
pub extern "C" fn ffi_drop_cfg(cfg: *const Cfg) -> () {
    unsafe {
        Box::from_raw(cfg as *mut Cfg);
    };
}

#[no_mangle]
// TODO Return result memory (let Node.js manage GC) instead of overwriting source.
pub extern "C" fn ffi_in_place(
    code: *mut u8,
    code_len: usize,
    cfg: *const Cfg,
    out_min_len: *mut usize,
) {
    let code_slice = unsafe { slice::from_raw_parts_mut(code, code_len) };
    let min_code = minify(code_slice, unsafe { &*cfg });
    let min_len = min_code.len();
    code_slice[..min_len].copy_from_slice(&min_code);
    unsafe {
        *out_min_len = min_len;
    }
}
