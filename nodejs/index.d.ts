export type Cfg = { __doNotUseCfgDirectly: string & { __itIsANapiExternalValue: never } };

/**
 * Creates configuration that can be passed to minification functions to change the minification approach.
 * The value returned is an [external](https://nodejs.org/api/n-api.html#n_api_napi_create_external) value with typeof `object` and prototype `null`; it does not have any properties, and should only be used as arguments to minify functions.
 *
 * @param options - Configurable minifier settings to use when this configuration is passed to a minify function
 * @returns An opaque value that can be passed to minify functions
 */
export function createConfiguration (options: {
  /** Do not omit closing tags when possible. */
  keep_closing_tags?: boolean;
  /** Do not omit `<html>` and `<head>` opening tags when they don't have attributes. */
  keep_html_and_head_opening_tags?: boolean;
  /** Keep spaces between attributes when possible to conform to HTML standards. */
  keep_spaces_between_attributes?: boolean;
  /** Keep all comments. */
  keep_comments?: boolean;
  /**
   * If enabled, content in `<script>` tags with a JS or no [MIME type](https://mimesniff.spec.whatwg.org/#javascript-mime-type) will be minified using [esbuild-rs](https://github.com/wilsonzlin/esbuild-rs).
   */
  minifyJs?: boolean;
  /**
   * If enabled, CSS in `<style>` tags will be minified using [esbuild-rs](https://github.com/wilsonzlin/esbuild-rs).
   */
  minifyCss?: boolean;
  /** Remove all bangs. */
  remove_bangs?: boolean;
  /** Remove all processing_instructions. */
  remove_processing_instructions?: boolean;
}): Cfg;

/**
 * Minifies a string containing HTML code.
 *
 * @param src - Source HTML code
 * @param cfg - Configuration created by {@link createConfiguration}
 * @returns Minified HTML code
 */
export function minify (src: string | Buffer, cfg: Cfg): Buffer;
