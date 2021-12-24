from typing import Optional

def minify(
    code: str,
    do_not_minify_doctype: Optional[bool],
    ensure_spec_compliant_unquoted_attribute_values: Optional[bool],
    keep_closing_tags: Optional[bool],
    keep_comments: Optional[bool],
    keep_html_and_head_opening_tags: Optional[bool],
    keep_spaces_between_attributes: Optional[bool],
    minify_css: Optional[bool],
    minify_js: Optional[bool],
    remove_bangs: Optional[bool],
    remove_processing_instructions: Optional[bool],
) -> str: ...
