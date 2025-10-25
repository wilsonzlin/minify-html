use neon::prelude::*;
use neon::types::buffer::TypedArray;

macro_rules! get_bool {
  ($cx:expr, $opt:expr, $name:literal) => {
    $opt
      .get_opt::<JsBoolean, _, _>(&mut $cx, $name)?
      .map(|v| v.value(&mut $cx))
      .unwrap_or(false)
  };
}

fn minify(mut cx: FunctionContext) -> JsResult<JsBuffer> {
  let Ok(src) = cx.try_catch(|cx| cx.argument::<JsBuffer>(0)) else {
    return cx.throw_type_error("the first argument is not a Buffer");
  };
  let Ok(opt) = cx.try_catch(|cx| cx.argument::<JsObject>(1)) else {
    return cx.throw_type_error("the second argument is not an object");
  };
  #[rustfmt::skip]
  let cfg = minify_html::Cfg {
    allow_noncompliant_unquoted_attribute_values: get_bool!(cx, opt, "allow_noncompliant_unquoted_attribute_values"),
    allow_optimal_entities: get_bool!(cx, opt, "allow_optimal_entities"),
    allow_removing_spaces_between_attributes: get_bool!(cx, opt, "allow_removing_spaces_between_attributes"),
    keep_closing_tags: get_bool!(cx, opt, "keep_closing_tags"),
    keep_comments: get_bool!(cx, opt, "keep_comments"),
    keep_html_and_head_opening_tags: get_bool!(cx, opt, "keep_html_and_head_opening_tags"),
    keep_input_type_text_attr: get_bool!(cx, opt, "keep_input_type_text_attr"),
    keep_ssi_comments: get_bool!(cx, opt, "keep_ssi_comments"),
    minify_css: get_bool!(cx, opt, "minify_css"),
    minify_doctype: get_bool!(cx, opt, "minify_doctype"),
    minify_js: get_bool!(cx, opt, "minify_js"),
    preserve_brace_template_syntax: get_bool!(cx, opt, "preserve_brace_template_syntax"),
    preserve_chevron_percent_template_syntax: get_bool!(cx, opt, "preserve_chevron_percent_template_syntax"),
    preserve_attribute_case: get_bool!(cx, opt, "preserve_attribute_case"),
    remove_bangs: get_bool!(cx, opt, "remove_bangs"),
    remove_processing_instructions: get_bool!(cx, opt, "remove_processing_instructions"),
  };
  let out = minify_html::minify(src.as_slice(&cx), &cfg);
  JsBuffer::from_slice(&mut cx, &out)
}

#[neon::main]
fn main(mut cx: ModuleContext) -> NeonResult<()> {
  cx.export_function("minify", minify)?;
  Ok(())
}
