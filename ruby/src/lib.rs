use minify_html::{Cfg, Error, in_place as minify_html_native};
use rutie::{Boolean, Class, class, Hash, methods, Object, RString, Symbol, VM};
use std::str::from_utf8_unchecked;

class!(MinifyHtml);

methods! {
    MinifyHtml,
    _itself,

    fn minify(source: RString, cfg_hash: Hash) -> RString {
        let mut code = source
            .map_err(|e| VM::raise_ex(e) )
            .unwrap()
            .to_string()
            .into_bytes();

        let cfg_hash = cfg_hash
            .map_err(|e| VM::raise_ex(e) )
            .unwrap();

        let cfg = &Cfg {
            minify_js: cfg_hash
                .at(&Symbol::new("minify_js"))
                .try_convert_to::<Boolean>()
                .map_or(false, |v| v.to_bool()),
            minify_css: cfg_hash
                .at(&Symbol::new("minify_css"))
                .try_convert_to::<Boolean>()
                .map_or(false, |v| v.to_bool()),
        };

        minify_html_native(&mut code, cfg)
            .map_err(|Error { error_type, position }| VM::raise(Class::from_existing("SyntaxError"), format!("{} [Character {}]", error_type.message(), position).as_str()))
            .map(|out_len| RString::new_utf8(unsafe { from_utf8_unchecked(&code[0..out_len]) }))
            .unwrap()
    }
}

#[allow(non_snake_case)]
#[no_mangle]
pub extern "C" fn Init_minify_html() {
    Class::new("MinifyHtml", None).define(|itself| {
        itself.def_self("minify", minify);
    });
}
