use neon::prelude::*;
use minify_html::{Cfg, Error, in_place};

fn minify(mut cx: FunctionContext) -> JsResult<JsNumber> {
    let mut buffer = cx.argument::<JsBuffer>(0)?;
    let cfg_obj = cx.argument::<JsObject>(1)?;
    let cfg = Cfg {
        minify_js: cfg_obj.get(&mut cx, "minifyJs")?.downcast::<JsBoolean>().or_throw(&mut cx)?.value(),
    };
    match cx.borrow_mut(&mut buffer, |code| in_place(code.as_mut_slice::<u8>(), &cfg)) {
        Ok(out_len) => Ok(cx.number(out_len as f64)),
        Err(Error { error_type, position }) => cx.throw_error(format!("{} [Character {}]", error_type.message(), position)),
    }
}

register_module!(mut cx, {
    cx.export_function("minify", minify)
});
