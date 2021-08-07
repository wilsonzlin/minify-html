use minify_html::{minify as minify_html_native, Cfg};
use jni::JNIEnv;
use jni::objects::{ JClass, JObject, JString};
use jni::sys::{ jstring};
use std::str::from_utf8;

fn build_cfg(
    env: &JNIEnv,
    obj: &JObject,
) -> Cfg {
    Cfg {
        ensure_spec_compliant_unquoted_attribute_values: env.get_field(*obj, "ensure_spec_compliant_unquoted_attribute_values", "Z").unwrap().z().unwrap(),
        keep_closing_tags: env.get_field(*obj, "keep_closing_tags", "Z").unwrap().z().unwrap(),
        keep_comments: env.get_field(*obj, "keep_comments", "Z").unwrap().z().unwrap(),
        keep_html_and_head_opening_tags: env.get_field(*obj, "keep_html_and_head_opening_tags", "Z").unwrap().z().unwrap(),
        keep_spaces_between_attributes: env.get_field(*obj, "keep_spaces_between_attributes", "Z").unwrap().z().unwrap(),
        minify_css: env.get_field(*obj, "minify_css", "Z").unwrap().z().unwrap(),
        minify_js: env.get_field(*obj, "minify_js", "Z").unwrap().z().unwrap(),
        remove_bangs: env.get_field(*obj, "remove_bangs", "Z").unwrap().z().unwrap(),
        remove_processing_instructions: env.get_field(*obj, "remove_processing_instructions", "Z").unwrap().z().unwrap(),
    }
}

#[no_mangle]
pub extern "system" fn Java_in_wilsonl_minifyhtml_MinifyHtml_minify(
    env: JNIEnv,
    _class: JClass,
    input: JString,
    cfg: JObject,
)
    -> jstring {
    let source: String = env.get_string(input).unwrap().into();
    let code = source.into_bytes();

    let out_code = minify_html_native(&code, &build_cfg(&env, &cfg));
    let out_code_str = from_utf8(&out_code).unwrap();
    env.new_string(out_code_str).unwrap().into_inner()
}
