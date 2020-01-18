use hyperbuild::hyperbuild;
use jni::JNIEnv;
use jni::objects::JClass;
use jni::sys::{jbyteArray, jint};

#[no_mangle]
pub extern "system" fn Java_in_wilsonl_hyperbuild_Hyperbuild_minifyInPlace(
    env: JNIEnv,
    _class: JClass,
    input: jbyteArray,
)
    -> jint {
    let source = &mut env.convert_byte_array(input).unwrap();

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
