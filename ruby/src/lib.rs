use minify_html::{minify as minify_html_native, Cfg};
use rutie::{class, methods, Boolean, Class, Hash, Object, RString, Symbol, VM};
use std::str::from_utf8;

macro_rules! get_cfg_hash_prop {
    ($cfg_hash:ident, $prop:literal) => {
        $cfg_hash
            .at(&Symbol::new($prop))
            .try_convert_to::<Boolean>()
            .map_or(false, |v| v.to_bool())
    };
}

class!(MinifyHtml);

methods! {
    MinifyHtml,
    _itself,

    fn minify(source: RString, cfg_hash: Hash) -> RString {
        let code = source
            .map_err(|e| VM::raise_ex(e) )
            .unwrap()
            .to_string()
            .into_bytes();

        let cfg_hash = cfg_hash
            .map_err(|e| VM::raise_ex(e) )
            .unwrap();

        let cfg = &Cfg {
            ensure_spec_compliant_unquoted_attribute_values: get_cfg_hash_prop!(cfg_hash, "ensure_spec_compliant_unquoted_attribute_values"),
            keep_closing_tags: get_cfg_hash_prop!(cfg_hash, "keep_closing_tags"),
            keep_comments: get_cfg_hash_prop!(cfg_hash, "keep_comments"),
            keep_html_and_head_opening_tags: get_cfg_hash_prop!(cfg_hash, "keep_html_and_head_opening_tags"),
            keep_spaces_between_attributes: get_cfg_hash_prop!(cfg_hash, "keep_spaces_between_attributes"),
            minify_css: get_cfg_hash_prop!(cfg_hash, "minify_css"),
            minify_js: get_cfg_hash_prop!(cfg_hash, "minify_js"),
            remove_bangs: get_cfg_hash_prop!(cfg_hash, "remove_bangs"),
            remove_processing_instructions: get_cfg_hash_prop!(cfg_hash, "remove_processing_instructions"),
        };

        let out_code = minify_html_native(&code, cfg);
        let out_str = from_utf8(&out_code).unwrap();
        RString::new_utf8(out_str)
    }
}

#[allow(non_snake_case)]
#[no_mangle]
pub extern "C" fn Init_minify_html() {
    Class::new("MinifyHtml", None).define(|itself| {
        itself.def_self("minify", minify);
    });
}
