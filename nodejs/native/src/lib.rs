use neon::prelude::*;
use hyperbuild::{Cfg, hyperbuild};

fn minify(mut cx: FunctionContext) -> JsResult<JsNumber> {
    let mut buffer = cx.argument::<JsBuffer>(0)?;
    let cfg_obj = cx.argument::<JsObject>(1)?;
    let cfg = Cfg {
        minify_js: cfg_obj.get(&mut cx, "minifyJs")?.downcast::<JsBoolean>().or_throw(&mut cx)?.value(),
    };
    match cx.borrow_mut(&mut buffer, |code| hyperbuild(code.as_mut_slice::<u8>(), &cfg)) {
        Ok(out_len) => Ok(cx.number(out_len as f64)),
        Err((err, pos)) => cx.throw_error(format!("{} [Character {}]", err.message(), pos)),
    }
}

register_module!(mut cx, {
    cx.export_function("minify", minify)
});
