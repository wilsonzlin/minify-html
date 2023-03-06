use std::slice;

use minify_html::Cfg;

#[no_mangle]
pub extern "C" fn minify_html(
    input: *const u8,
    length: usize,
    do_not_minify_doctype: bool,
    ensure_spec_compliant_unquoted_attribute_values: bool,
    keep_closing_tags: bool,
    keep_html_and_head_opening_tags: bool,
    keep_spaces_between_attributes: bool,
    keep_comments: bool,
    minify_css: bool,
    minify_css_level_1: bool,
    minify_css_level_2: bool,
    minify_css_level_3: bool,
    minify_js: bool,
    remove_bangs: bool,
    remove_processing_instructions: bool,
) {
    let src = unsafe { slice::from_raw_parts(input, length) };

    let cfg = Cfg {
        do_not_minify_doctype,
        ensure_spec_compliant_unquoted_attribute_values,
        keep_closing_tags,
        keep_html_and_head_opening_tags,
        keep_spaces_between_attributes,
        keep_comments,
        minify_css,
        minify_css_level_1,
        minify_css_level_2,
        minify_css_level_3,
        minify_js,
        remove_bangs,
        remove_processing_instructions,
    };

    minify_html::minify(src, &cfg);
}
