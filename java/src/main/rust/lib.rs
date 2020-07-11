use minify_html::{in_place as minify_html_native, Cfg};
use jni::JNIEnv;
use jni::objects::{JByteBuffer, JClass, JObject, JString};
use jni::sys::{jint, jstring};
use std::str::from_utf8_unchecked;

fn build_cfg(
    env: &JNIEnv,
    obj: &JObject,
) -> Cfg {
    Cfg {
        minify_js: env.get_field(*obj, "minifyJs", "Z").unwrap().z().unwrap(),
    }
}

#[no_mangle]
pub extern "system" fn Java_in_wilsonl_minify_1html_MinifyHtml_minifyInPlace(
    env: JNIEnv,
    _class: JClass,
    input: JByteBuffer,
    cfg: JObject,
)
    -> jint {
    let source = match env.get_direct_buffer_address(input) {
        Ok(ptr) => ptr,
        Err(_) => {
            env.throw_new("java/lang/IllegalArgumentException", "ByteBuffer is not direct").unwrap();
            return 0;
        }
    };

    (match minify_html_native(source, &build_cfg(&env, &cfg)) {
        Ok(out_len) => out_len,
        Err((err, pos)) => {
            env.throw_new(
                "in/wilsonl/minify_html/MinifyHtml$SyntaxException",
                format!("{} [Character {}]", err.message(), pos),
            ).unwrap();
            0
        }
    }) as jint
}

#[no_mangle]
pub extern "system" fn Java_in_wilsonl_minify_1html_MinifyHtml_minify(
    env: JNIEnv,
    _class: JClass,
    input: JString,
    cfg: JObject,
)
    -> jstring {
    let source: String = env.get_string(input).unwrap().into();
    let mut code = source.into_bytes();

    match minify_html_native(&mut code, &build_cfg(&env, &cfg)) {
        Ok(out_len) => env.new_string(unsafe { from_utf8_unchecked(&code[0..out_len]) }).unwrap().into_inner(),
        Err((err, pos)) => {
            env.throw_new(
                "in/wilsonl/minify_html/MinifyHtml$SyntaxException",
                format!("{} [Character {}]", err.message(), pos),
            ).unwrap();
            JObject::null().into_inner()
        }
    }
}
