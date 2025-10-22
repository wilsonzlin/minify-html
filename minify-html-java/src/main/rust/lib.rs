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
    // BEGIN CONFIGURATION FIELDS
    allow_noncompliant_unquoted_attribute_values: env.call_method(*obj, "isAllowNoncompliantUnquotedAttributeValues", "()Z", &[]).unwrap().z().unwrap(),
    allow_optimal_entities: env.call_method(*obj, "isAllowOptimalEntities", "()Z", &[]).unwrap().z().unwrap(),
    allow_removing_spaces_between_attributes: env.call_method(*obj, "isAllowRemovingSpacesBetweenAttributes", "()Z", &[]).unwrap().z().unwrap(),
    keep_closing_tags: env.call_method(*obj, "isKeepClosingTags", "()Z", &[]).unwrap().z().unwrap(),
    keep_comments: env.call_method(*obj, "isKeepComments", "()Z", &[]).unwrap().z().unwrap(),
    keep_html_and_head_opening_tags: env.call_method(*obj, "isKeepHtmlAndHeadOpeningTags", "()Z", &[]).unwrap().z().unwrap(),
    keep_input_type_text_attr: env.call_method(*obj, "isKeepInputTypeTextAttr", "()Z", &[]).unwrap().z().unwrap(),
    keep_ssi_comments: env.call_method(*obj, "isKeepSsiComments", "()Z", &[]).unwrap().z().unwrap(),
    minify_css: env.call_method(*obj, "isMinifyCss", "()Z", &[]).unwrap().z().unwrap(),
    minify_doctype: env.call_method(*obj, "isMinifyDoctype", "()Z", &[]).unwrap().z().unwrap(),
    minify_js: env.call_method(*obj, "isMinifyJs", "()Z", &[]).unwrap().z().unwrap(),
    preserve_brace_template_syntax: env.call_method(*obj, "isPreserveBraceTemplateSyntax", "()Z", &[]).unwrap().z().unwrap(),
    preserve_chevron_percent_template_syntax: env.call_method(*obj, "isPreserveChevronPercentTemplateSyntax", "()Z", &[]).unwrap().z().unwrap(),
    remove_bangs: env.call_method(*obj, "isRemoveBangs", "()Z", &[]).unwrap().z().unwrap(),
    remove_processing_instructions: env.call_method(*obj, "isRemoveProcessingInstructions", "()Z", &[]).unwrap().z().unwrap(),
    // END CONFIGURATION FIELDS
  };
  cfg
}

#[no_mangle]
pub extern "system" fn Java_in_wilsonl_minifyhtml_MinifyHtml_minifyRs(
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
