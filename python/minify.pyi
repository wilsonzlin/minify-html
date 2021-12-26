def minify(
    code: str,
    do_not_minify_doctype: bool = False,
    ensure_spec_compliant_unquoted_attribute_values: bool = False,
    keep_closing_tags: bool = False,
    keep_comments: bool = False,
    keep_html_and_head_opening_tags: bool = False,
    keep_spaces_between_attributes: bool = False,
    minify_css: bool = False,
    minify_js: bool = False,
    remove_bangs: bool = False,
    remove_processing_instructions: bool = False,
) -> str: ...
