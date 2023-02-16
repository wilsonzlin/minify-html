use neon::prelude::*;
use neon::types::buffer::TypedArray;

macro_rules! get_bool {
    ($cx:expr, $opt:expr, $name:literal) => {
        $opt.get_opt::<JsBoolean, _, _>(&mut $cx, $name)?
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
    let cfg = minify_html::Cfg {
        do_not_minify_doctype: get_bool!(cx, opt, "do_not_minify_doctype"),
        ensure_spec_compliant_unquoted_attribute_values: get_bool!(
            cx,
            opt,
            "ensure_spec_compliant_unquoted_attribute_values"
        ),
        keep_closing_tags: get_bool!(cx, opt, "keep_closing_tags"),
        keep_html_and_head_opening_tags: get_bool!(cx, opt, "keep_html_and_head_opening_tags"),
        keep_spaces_between_attributes: get_bool!(cx, opt, "keep_spaces_between_attributes"),
        keep_comments: get_bool!(cx, opt, "keep_comments"),
        minify_css: get_bool!(cx, opt, "minify_css"),
        minify_css_level_1: get_bool!(cx, opt, "minify_css_level_1"),
        minify_css_level_2: get_bool!(cx, opt, "minify_css_level_2"),
        minify_css_level_3: get_bool!(cx, opt, "minify_css_level_3"),
        minify_js: get_bool!(cx, opt, "minify_js"),
        remove_bangs: get_bool!(cx, opt, "remove_bangs"),
        remove_processing_instructions: get_bool!(cx, opt, "remove_processing_instructions"),
    };
    let out = minify_html::minify(src.as_slice(&mut cx), &cfg);
    Ok(JsBuffer::external(&mut cx, out))
}

#[neon::main]
fn main(mut cx: ModuleContext) -> NeonResult<()> {
    cx.export_function("minify", minify)?;
    Ok(())
}
