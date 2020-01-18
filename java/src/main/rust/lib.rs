use hyperbuild::hyperbuild;
use jni::JNIEnv;
use jni::objects::{JByteBuffer, JClass, JObject, JString};
use jni::sys::{jint, jstring};
use std::str::from_utf8_unchecked;

#[no_mangle]
pub extern "system" fn Java_in_wilsonl_hyperbuild_Hyperbuild_minifyInPlace(
    env: JNIEnv,
    _class: JClass,
    input: JByteBuffer,
)
    -> jint {
    let source = match env.get_direct_buffer_address(input) {
        Ok(ptr) => ptr,
        Err(_) => {
            env.throw_new("java/lang/IllegalArgumentException", "ByteBuffer is not direct").unwrap();
            return 0;
        }
    };

    (match hyperbuild(source) {
        Ok(out_len) => out_len,
        Err((err, pos)) => {
            env.throw_new(
                "in/wilsonl/hyperbuild/HyperbuildException",
                format!("{} [Character {}]", err.message(), pos),
            ).unwrap();
            0
        }
    }) as jint
}

#[no_mangle]
pub extern "system" fn Java_in_wilsonl_hyperbuild_Hyperbuild_minify(
    env: JNIEnv,
    _class: JClass,
    input: JString,
)
    -> jstring {
    let source: String = env.get_string(input).unwrap().into();
    let mut code = source.as_bytes().to_vec();

    match hyperbuild(&mut code) {
        Ok(out_len) => env.new_string(unsafe { from_utf8_unchecked(&code[0..out_len]) }).unwrap().into_inner(),
        Err((err, pos)) => {
            env.throw_new(
                "in/wilsonl/hyperbuild/HyperbuildException",
                format!("{} [Character {}]", err.message(), pos),
            ).unwrap();
            JObject::null().into_inner()
        }
    }
}
