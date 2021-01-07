export type Cfg = { __doNotUseCfgDirectly: string & { __itIsANapiExternalValue: never } };

/**
 * Creates configuration that can be passed to minification functions to change the minification approach.
 * The value returned is an [external](https://nodejs.org/api/n-api.html#n_api_napi_create_external) value with typeof `object` and prototype `null`; it does not have any properties, and should only be used as arguments to minify functions.
 *
 * @param options - Configurable minifier settings to use when this configuration is passed to a minify function
 * @returns An opaque value that can be passed to minify functions
 */
export function createConfiguration (options: {
  /**
   * If enabled, content in `<script>` tags with a JS or no [MIME type](https://mimesniff.spec.whatwg.org/#javascript-mime-type) will be minified using [esbuild-rs](https://github.com/wilsonzlin/esbuild-rs).
   */
  minifyJs: boolean;
  /**
   * If enabled, CSS in `<style>` tags will be minified using [esbuild-rs](https://github.com/wilsonzlin/esbuild-rs).
   */
  minifyCss: boolean;
}): Cfg;

/**
 * Minifies a string containing HTML code.
 * Throws an {@link Error} if the source code cannot be minified, with a `position` property representing the position of the character in the source code that caused the error.
 *
 * @param src - Source HTML code
 * @param cfg - Configuration created by {@link createConfiguration}
 * @returns Minified HTML code
 */
export function minify (src: string, cfg: Cfg): Buffer;

/**
 * Minifies a {@link Buffer} containing UTF-8 HTML code in place.
 * Throws an {@link Error} if the source code cannot be minified, with a `position` property representing the position of the character in the source code that caused the error.
 *
 * @param code - Source Buffer code
 * @param cfg - Configuration created by {@link createConfiguration}
 * @returns Subslice of source Buffer (not a copy) representing the minified output HTML
 */
export function minifyInPlace (code: Buffer, cfg: Cfg): Buffer;
