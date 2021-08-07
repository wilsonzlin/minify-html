use minify_html::{minify, Cfg};
use std::{mem, slice};

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

#[repr(C)]
pub struct ffi_output {
    data: *mut u8,
    len: usize,
    cap: usize,
}

#[no_mangle]
pub extern "C" fn ffi_drop_output(ptr: *const ffi_output) -> () {
    unsafe {
        let out = Box::from_raw(ptr as *mut ffi_output);
        Vec::from_raw_parts(out.data, out.len, out.cap);
    };
}

#[no_mangle]
// TODO Return result memory (let Node.js manage GC) instead of overwriting source.
pub extern "C" fn ffi_minify(
    code: *const u8,
    code_len: usize,
    cfg: *const Cfg,
) -> *const ffi_output {
    let code_slice = unsafe { slice::from_raw_parts(code, code_len) };
    let mut out_code = minify(code_slice, unsafe { &*cfg });
    let res = Box::into_raw(Box::new(ffi_output {
        data: out_code.as_mut_ptr(),
        len: out_code.len(),
        cap: out_code.capacity(),
    }));
    mem::forget(out_code);
    res
}
