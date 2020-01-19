use hyperbuild::hyperbuild as hyperbuild_native;
use rutie::{Class, class, methods, Object, RString, VM};
use std::str::from_utf8_unchecked;

class!(Hyperbuild);

methods! {
    Hyperbuild,
    _itself,

    fn minify(source: RString) -> RString {
        let mut code = source
            .map_err(|e| VM::raise_ex(e) )
            .unwrap()
            .to_string()
            .into_bytes();

        hyperbuild_native(&mut code)
            .map_err(|(err, pos)| VM::raise(Class::from_existing("SyntaxError"), format!("{} [Character {}]", err.message(), pos).as_str()))
            .map(|out_len| RString::new_utf8(unsafe { from_utf8_unchecked(&code[0..out_len]) }))
            .unwrap()
    }
}

#[allow(non_snake_case)]
#[no_mangle]
pub extern "C" fn Init_hyperbuild() {
    Class::new("Hyperbuild", None).define(|itself| {
        itself.def_self("minify", minify);
    });
}
