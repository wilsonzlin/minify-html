use std::{mem, ptr, slice};
use minify_html::{Cfg, Error, in_place};

#[no_mangle]
pub extern "C" fn ffi_create_cfg(minify_js: bool, minify_css: bool) -> *const Cfg {
    Box::into_raw(Box::new(Cfg {
        minify_js,
        minify_css,
    }))
}

#[no_mangle]
pub extern "C" fn ffi_drop_cfg(cfg: *const Cfg) -> () {
    unsafe {
        Box::from_raw(cfg as *mut Cfg);
    };
}

#[repr(C)]
pub struct ffi_error {
    message: *mut u8,
    message_len: usize,
    position: usize,
}

#[no_mangle]
pub extern "C" fn ffi_drop_ffi_error(ffi_error_ptr: *const ffi_error) -> () {
    unsafe {
        let ffi_error = Box::from_raw(ffi_error_ptr as *mut ffi_error);
        let _ = String::from_raw_parts(ffi_error.message, ffi_error.message_len, ffi_error.message_len);
    };
}

#[no_mangle]
pub extern "C" fn ffi_in_place(code: *mut u8, code_len: usize, cfg: *const Cfg, out_min_len: *mut usize) -> *const ffi_error {
    let code_slice = unsafe { slice::from_raw_parts_mut(code, code_len) };
    match in_place(code_slice, unsafe { &*cfg }) {
        Ok(min_len) => unsafe {
            *out_min_len = min_len;
            ptr::null()
        }
        Err(Error { error_type, position }) => {
            let mut msg = error_type.message();
            msg.shrink_to_fit();
            let msg_ptr = msg.as_mut_ptr();
            let msg_len = msg.len();
            mem::forget(msg);
            Box::into_raw(Box::new(ffi_error {
                message: msg_ptr,
                message_len: msg_len,
                position,
            }))
        }
    }
}
