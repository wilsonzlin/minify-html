#[macro_use]
extern crate neon;

use neon::prelude::*;
use hyperbuild::hyperbuild;

fn minify(mut cx: FunctionContext) -> JsResult<JsNumber> {
    let mut buffer = cx.argument::<JsBuffer>(0)?;
    match cx.borrow_mut(&mut buffer, |mut code| hyperbuild(code.as_mut_slice::<u8>())) {
        Ok(out_len) => Ok(cx.number(out_len as f64)),
        Err((err, pos)) => cx.throw_error(format!("{} [Character {}]", err.message(), pos)),
    }
}

register_module!(mut cx, {
    cx.export_function("minify", minify)
});
