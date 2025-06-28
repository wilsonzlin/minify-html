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
    allow_noncompliant_unquoted_attribute_values: env.call_method(*obj, "getAllowNoncompliantUnquotedAttributeValues", "()Z", &[]).unwrap().z().unwrap(),
    allow_optimal_entities: env.call_method(*obj, "getAllowOptimalEntities", "()Z", &[]).unwrap().z().unwrap(),
    allow_removing_spaces_between_attributes: env.call_method(*obj, "getAllowRemovingSpacesBetweenAttributes", "()Z", &[]).unwrap().z().unwrap(),
    keep_closing_tags: env.call_method(*obj, "getKeepClosingTags", "()Z", &[]).unwrap().z().unwrap(),
    keep_comments: env.call_method(*obj, "getKeepComments", "()Z", &[]).unwrap().z().unwrap(),
    keep_html_and_head_opening_tags: env.call_method(*obj, "getKeepHtmlAndHeadOpeningTags", "()Z", &[]).unwrap().z().unwrap(),
    keep_input_type_text_attr: env.call_method(*obj, "getKeepInputTypeTextAttr", "()Z", &[]).unwrap().z().unwrap(),
    keep_ssi_comments: env.call_method(*obj, "getKeepSsiComments", "()Z", &[]).unwrap().z().unwrap(),
    minify_css: env.call_method(*obj, "getMinifyCss", "()Z", &[]).unwrap().z().unwrap(),
    minify_doctype: env.call_method(*obj, "getMinifyDoctype", "()Z", &[]).unwrap().z().unwrap(),
    minify_js: env.call_method(*obj, "getMinifyJs", "()Z", &[]).unwrap().z().unwrap(),
    preserve_brace_template_syntax: env.call_method(*obj, "getPreserveBraceTemplateSyntax", "()Z", &[]).unwrap().z().unwrap(),
    preserve_chevron_percent_template_syntax: env.call_method(*obj, "getPreserveChevronPercentTemplateSyntax", "()Z", &[]).unwrap().z().unwrap(),
    remove_bangs: env.call_method(*obj, "getRemoveBangs", "()Z", &[]).unwrap().z().unwrap(),
    remove_processing_instructions: env.call_method(*obj, "getRemoveProcessingInstructions", "()Z", &[]).unwrap().z().unwrap(),
  };
  cfg
}

#[no_mangle]
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
