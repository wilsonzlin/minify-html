/**
 * Minifies a Buffer containing UTF-8 HTML code.
 *
 * @param src - Source HTML code
 * @param cfg - Configurable minifier settings to use
 * @returns Minified HTML code
 */
export function minify(
  src: Buffer,
  cfg: {
    /** Allow unquoted attribute values in the output to contain characters prohibited by the [WHATWG specification](https://html.spec.whatwg.org/multipage/syntax.html#attributes-2). These will still be parsed correctly by almost all browsers. */
    allow_noncompliant_unquoted_attribute_values?: boolean;
    /** Allow some minifications around entities that may not pass validation, but will still be parsed correctly by almost all browsers. */
    allow_optimal_entities?: boolean;
    /** Allow removing_spaces between attributes when possible, which may not be spec compliant. These will still be parsed correctly by almost all browsers. */
    allow_removing_spaces_between_attributes?: boolean;
    /** Do not omit closing tags when possible. */
    keep_closing_tags?: boolean;
    /** Keep all comments. */
    keep_comments?: boolean;
    /** Do not omit `<html>` and `<head>` opening tags when they don't have attributes. */
    keep_html_and_head_opening_tags?: boolean;
    /** Keep `type=text` attribute name and value on `<input>` elements. */
    keep_input_type_text_attr?: boolean;
    /** Keep SSI comments. */
    keep_ssi_comments?: boolean;
    /** Minify CSS in `<style>` tags and `style` attributes using [https://github.com/parcel-bundler/lightningcss](lightningcss). */
    minify_css?: boolean;
    /** Minify DOCTYPEs. Minified DOCTYPEs may not be spec compliant, but will still be parsed correctly by almost all browsers. */
    minify_doctype?: boolean;
    /** Minify JavaScript in `<script>` tags using [minify-js](https://github.com/wilsonzlin/minify-js). */
    minify_js?: boolean;
    /** When `{{`, `{#`, or `{%` are seen in content, all source code until the subsequent matching closing `}}`, `#}`, or `%}` respectively gets piped through untouched. */
    preserve_brace_template_syntax?: boolean;
    /** When `<%` is seen in content, all source code until the subsequent matching closing `%>` gets piped through untouched. */
    preserve_chevron_percent_template_syntax?: boolean;
    /** Remove all bangs. */
    remove_bangs?: boolean;
    /** Remove all processing instructions. */
    remove_processing_instructions?: boolean;
  }
): Buffer;
