use jni::objects::JClass;
use jni::objects::JObject;
use jni::objects::JString;
use jni::sys::jstring;
use jni::JNIEnv;
use minify_html::minify as minify_html_native;
use minify_html::Cfg;
use std::str::from_utf8;

fn build_cfg(env: &JNIEnv, obj: &JObject) -> Cfg {
  #[rustfmt::skip]
  // This is a statement because "attributes on expressions are experimental".
  let cfg = Cfg {
    allow_noncompliant_unquoted_attribute_values: env.get_field(*obj, "allow_noncompliant_unquoted_attribute_values", "Z").unwrap().z().unwrap(),
    allow_optimal_entities: env.get_field(*obj, "allow_optimal_entities", "Z").unwrap().z().unwrap(),
    allow_removing_spaces_between_attributes: env.get_field(*obj, "allow_removing_spaces_between_attributes", "Z").unwrap().z().unwrap(),
    keep_closing_tags: env.get_field(*obj, "keep_closing_tags", "Z").unwrap().z().unwrap(),
    keep_comments: env.get_field(*obj, "keep_comments", "Z").unwrap().z().unwrap(),
    keep_html_and_head_opening_tags: env.get_field(*obj, "keep_html_and_head_opening_tags", "Z").unwrap().z().unwrap(),
    keep_input_type_text_attr: env.get_field(*obj, "keep_input_type_text_attr", "Z").unwrap().z().unwrap(),
    keep_ssi_comments: env.get_field(*obj, "keep_ssi_comments", "Z").unwrap().z().unwrap(),
    minify_css: env.get_field(*obj, "minify_css", "Z").unwrap().z().unwrap(),
    minify_doctype: env.get_field(*obj, "minify_doctype", "Z").unwrap().z().unwrap(),
    minify_js: env.get_field(*obj, "minify_js", "Z").unwrap().z().unwrap(),
    preserve_brace_template_syntax: env.get_field(*obj, "preserve_brace_template_syntax", "Z").unwrap().z().unwrap(),
    preserve_chevron_percent_template_syntax: env.get_field(*obj, "preserve_chevron_percent_template_syntax", "Z").unwrap().z().unwrap(),
    remove_bangs: env.get_field(*obj, "remove_bangs", "Z").unwrap().z().unwrap(),
    remove_processing_instructions: env.get_field(*obj, "remove_processing_instructions", "Z").unwrap().z().unwrap(),
  };
  cfg
}

#[unsafe(no_mangle)]
pub extern "system" fn Java_in_wilsonl_minifyhtml_MinifyHtml_minify(
  env: JNIEnv,
  _class: JClass,
  input: JString,
  cfg: JObject,
) -> jstring {
  let source: String = env.get_string(input).unwrap().into();
  let code = source.into_bytes();

  let out_code = minify_html_native(&code, &build_cfg(&env, &cfg));
  let out_code_str = from_utf8(&out_code).unwrap();
  env.new_string(out_code_str).unwrap().into_inner()
}
