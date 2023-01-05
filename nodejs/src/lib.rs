use neon::prelude::*;
use neon::types::buffer::TypedArray;

fn minify(mut cx: FunctionContext) -> JsResult<JsBuffer> {
    let Ok(src) = cx.argument::<JsBuffer>(0) else {
      return cx.throw_type_error("the first argument is not a Buffer");
    };
    let Ok(opt) = cx.argument::<JsObject>(1) else {
      return cx.throw_type_error("the second argument is not an object");
    };
    let cfg = minify_html::Cfg {
        do_not_minify_doctype: opt
            .get_opt::<JsBoolean, _, _>(&mut cx, "do_not_minify_doctype")?
            .map(|v| v.value(&mut cx))
            .unwrap_or(false),
        ensure_spec_compliant_unquoted_attribute_values: opt
            .get_opt::<JsBoolean, _, _>(&mut cx, "ensure_spec_compliant_unquoted_attribute_values")?
            .map(|v| v.value(&mut cx))
            .unwrap_or(false),
        keep_closing_tags: opt
            .get_opt::<JsBoolean, _, _>(&mut cx, "keep_closing_tags")?
            .map(|v| v.value(&mut cx))
            .unwrap_or(false),
        keep_html_and_head_opening_tags: opt
            .get_opt::<JsBoolean, _, _>(&mut cx, "keep_html_and_head_opening_tags")?
            .map(|v| v.value(&mut cx))
            .unwrap_or(false),
        keep_spaces_between_attributes: opt
            .get_opt::<JsBoolean, _, _>(&mut cx, "keep_spaces_between_attributes")?
            .map(|v| v.value(&mut cx))
            .unwrap_or(false),
        keep_comments: opt
            .get_opt::<JsBoolean, _, _>(&mut cx, "keep_comments")?
            .map(|v| v.value(&mut cx))
            .unwrap_or(false),
        minify_css: opt
            .get_opt::<JsBoolean, _, _>(&mut cx, "minify_css")?
            .map(|v| v.value(&mut cx))
            .unwrap_or(false),
        minify_js: opt
            .get_opt::<JsBoolean, _, _>(&mut cx, "minify_js")?
            .map(|v| v.value(&mut cx))
            .unwrap_or(false),
        remove_bangs: opt
            .get_opt::<JsBoolean, _, _>(&mut cx, "remove_bangs")?
            .map(|v| v.value(&mut cx))
            .unwrap_or(false),
        remove_processing_instructions: opt
            .get_opt::<JsBoolean, _, _>(&mut cx, "remove_processing_instructions")?
            .map(|v| v.value(&mut cx))
            .unwrap_or(false),
    };
    let out = minify_html::minify(src.as_slice(&mut cx), &cfg);
    Ok(JsBuffer::external(&mut cx, out))
}

#[neon::main]
fn main(mut cx: ModuleContext) -> NeonResult<()> {
    cx.export_function("minify", minify)?;
    Ok(())
}
