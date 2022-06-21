# minify-html changelog

## 0.9.1

- Fix Node.js postinstall script.

## 0.9.0

- Replace esbuild with [minify-js](https://github.com/wilsonzlin/minify-js) as the JS minifier, a fast minifier written from scratch in Rust. This alleviates many of the problems with integrating with esbuild, including interference with process signals by the Go runtime, compatibility issues with C libraries other than glibc, use of threading libraries without actually threading, inability to compile to rarer Rust targets, dependency on the Go compiler, maintaining a [fork of esbuild](https://github.com/wilsonzlin/esbuild-rs), unsafe FFI, and more. CSS minification is now done by [css-minify](https://github.com/Mnwa/css-minify).
  - As minify-js is a relatively new library, any feedback, suggestions, and issues around JS minification is most welcome! Please report them to [the repo](https://github.com/wilsonzlin/minify-js).
- Use [Neon](https://neon-bindings.com/) for the Node.js library instead of custom hand-written N-API bindings in C. This simplifies the code and makes it safer and easier to extend. It also allows building from source if a prebuilt binary is not available (the Rust compiler must be installed).
  - The package has been renamed to `@minify-js/node`.
  - There is a slight API change: instead of calling `createConfiguration`, directly pass the JavaScript object to the `minify` function. The `minify` function also no longer takes a string.
- Thanks to the change to the fully-Rust [minify-js](https://github.com/wilsonzlin/minify-js), we can now add support for Deno and WebAssembly.
- Due to the dropping of esbuild, there is no more `core` variant for Node.js and Python, as the issues should no longer exist.

## 0.8.1

- Create wrapper index.js for Node.js library to support ESM.
- Do not consider empty `value` attributes on `option` elements as redundant.
- Consider `crossorigin` attributes as boolean.

## 0.8.0

- Minify whitespace in SVG elements.

## 0.7.2

- Fix Node.js library build process on Windows.

## 0.7.1

- Do not remove `alt` attribute when empty.

## 0.7.0

- Fix Node.js library not including `cli.js`.
- [CLI] Add support for macOS ARM64.
- [Node.js] Add support for macOS ARM64.
- [Python] Add support for Linux ARM64 and macOS ARM64. Drop support for Python 3.7 (breaking change).

## 0.6.10

- Fix the Node.js library TypeScript definitions. `minifyJs` has been fixed to `minify_js` and `minifyCss` has been fixed to `minify_css`. This is not a breaking change the library itself only ever accepted the fixed names, so this is actually a typo fix.
- Implement a basic CLI script for Node.js to allow using the library from the command line e.g. quick testing or sandboxing without needing to download and install the CLI separately. It accepts all configuration properties (all of which are currently booleans) using hyphen case e.g. `--do-not-minify-doctype`, as well as `--output [path]` and one default (i.e. not after an option switch) argument for the path to the input. It's only a few lines long and should not have a tangible effect on library size.

## 0.6.9

- Intrepret `type=module` on `<script>` tags as a JavaScript MIME eligible for its contents to be minified as JavaScript (previously it would not be and so its contents would be considered data and never minified as JavaScript).
- Fix issue where spaces are not added between unquoted attributes even when `cfg.keep_spaces_between_attributes` is `true`.
