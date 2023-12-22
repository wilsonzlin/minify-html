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
    /** Do not minify DOCTYPEs. Minified DOCTYPEs may not be spec compliant. */
    do_not_minify_doctype?: boolean;
    /** Ensure all unquoted attribute values in the output do not contain any characters prohibited by the WHATWG specification. */
    ensure_spec_compliant_unquoted_attribute_values?: boolean;
    /** Do not omit closing tags when possible. */
    keep_closing_tags?: boolean;
    /** Do not omit `<html>` and `<head>` opening tags when they don't have attributes. */
    keep_html_and_head_opening_tags?: boolean;
    /** Keep spaces between attributes when possible to conform to HTML standards. */
    keep_spaces_between_attributes?: boolean;
    /** Keep all comments. */
    keep_comments?: boolean;
    /**
     * If enabled, content in `<script>` tags with a JS or no [MIME type](https://mimesniff.spec.whatwg.org/#javascript-mime-type) will be minified using [minify-js](https://github.com/wilsonzlin/minify-js).
     */
    minify_js?: boolean;
    /**
     * If enabled, CSS in `<style>` tags and `style` attributes will be minified.
     */
    minify_css?: boolean;
    /**
     * Use optimisation level 1 for the CSS minifier. This is currently the default, but may change in the future if higher levels become safe.
     */
    minify_css_level_1?: boolean;
    /**
     * Use optimisation level 2 for the CSS minifier. This is mostly safe, but may perform some dangerous optimisations.
     */
    minify_css_level_2?: boolean;
    /**
     * Use optimisation level 3 for the CSS minifier. This performs many dangerous optimisations, so ensure any input works with this level.
     */
    minify_css_level_3?: boolean;
    /** Remove all bangs. */
    remove_bangs?: boolean;
    /** Remove all processing_instructions. */
    remove_processing_instructions?: boolean;
  }
): Buffer;
