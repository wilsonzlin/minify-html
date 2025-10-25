# minify-html changelog

## Pending

- Fix WASM builds.

## 0.18.0

- Fix WASM builds.
- [Java] BREAKING: Minimum Java version increased from 7 to 8.

## 0.17.1

- Bump Ubuntu version for GitHub Actions.

## 0.17.0

- [Node.js] BREAKING: Minimum Node.js version increased from 8.6.0 to 10.0.0.
- [Ruby] BREAKING: Minimum Ruby version increased from 2.7 to 3.2.
- Replace minify-js with [oxc_minifier](https://github.com/oxc-project/oxc) for JavaScript minification.
- [Python] Add musl builds for Linux.
- Bump Windows version for GitHub Actions.
- Migrate to Central Portal for Java.

## 0.16.5

- Bump Java version for GitHub Actions.
- [CLI] Add missing exit status on I/O errors.
- [Internal] Loosen ahash dependency version.
- [Internal] Bump lightningcss.
- [Python] Add Python 3.14 support.

## 0.16.4

- Fix macOS label for GitHub Actions.

## 0.16.3

- Bump macOS version for GitHub Actions.

## 0.16.2

- Fix Node.js builds.
- Fix Deno builds.

## 0.16.1

- Fix Node.js builds.
- Fix Deno builds.

## 0.16.0

- BREAKING: Cfg options have changed such that spec compliance is the default, to avoid confusion with users:
  - `do_not_minify_doctype` => `minify_doctype`
  - `ensure_spec_compliant_unquoted_attribute_values` => `allow_noncompliant_unquoted_attribute_values`
  - `keep_spaces_between_attributes` => `allow_removing_spaces_between_attributes`
  - `Cfg::spec_compliant()` => `Cfg::enable_possibly_noncompliant(&mut self)`
- BREAKING: Some entity minifications are now classified as "possibly noncompliant" and can be enabled via the `allow_optimal_entities` option but won't be performed by default.
- [Internal] Migrate to [aHash](https://github.com/tkaitchuck/aHash/blob/master/compare/readme.md) for faster more consistent performance and once_cell for modern ergonomics.
- [Internal] Downgrade aho-corasick to 0.7 temporarily to support minify-js.
- [Node.js] Fix ARM64 package metadata.
- [Python] Add Python 3.13 support.
- [Ruby] Add Ruby 3.3/3.4 support.
- [Deno] Publish to JSR.
- [Rust] Deny unsafe code in minify-html crate.

## 0.15.0

- Add `keep_input_type_text_attr` option to keep `type=text` on `<input>` elements.
- [Java] The `Configuration` class constructor has been made private to enforce the use of the builder. The constructor has a lot of params which can easily cause bugs due to ordering and confusion.

## 0.14.0

- Add new options to parse and preserve common templating syntax in content source code. NOTE: The parsing is "dumb" and merely looks for the next subsequence in the source code that matches the closing delimiter characters. This means that literal closing delimiter characters (e.g. strings) and nesting may cause parsing to be incorrect.
  - `preserve_brace_template_syntax`: When `{{`, `{#`, or `{%` are seen in content, all source code until the subsequent matching closing `}}`, `#}`, or `%}` respectively gets piped through untouched.
    - Templating engines: Pebble, Mustache, Django, Go, Jinja, Twix, Nunjucks, Handlebars, Liquid.
  - `preserve_chevron_percent_template_syntax`: When `<%` is seen in content, all source code until the subsequent matching closing `%>` gets piped through untouched.
    - Templating engines: Sailfish, JSP, EJS, ERB.

## 0.13.3

- Avoid downloading html-data JSON from network on build.

## 0.13.2

- [Java] Set up cross compilation for macOS and Linux ARM64 builds.

## 0.13.1

- [CLI] Add missing Cargo metadata.

## 0.13.0

- Use [lightningcss](https://github.com/parcel-bundler/lightningcss) instead of css-minify, which is better maintained.
  - BREAKING: The `minify_css_level_*` Cfg options no longer apply and have been removed.
- [onepass] Implement `Display` and `Error` for `Error` and `FriendlyError` structs.

## 0.12.0

- Change CLI name to `minhtml` as it's a more concise command name and allows for `cargo install minhtml`.
- Add `keep_ssi_comments` to preserve SSI comments.
- [Ruby] BREAKING: The class method is now a global function, so call `minify_html` instead of `MinifyHtml.minify`. All else remains the same. This is due to migrating from Rutie (see [0.11.3](#0113)).
  - This change was inadvertently released in patch version bumps from `0.11.3` to `0.11.5`; these gems have been yanked.

## 0.11.5

- Omit Rust source files from Node.js package.

## 0.11.4

- Bump minify-js version.
- Fix Node.js native package names.

## 0.11.3

- Fix detection of module type scripts.
- Derive `Clone` for `Cfg` in minify-html.
- Fix parsing of malformed closing tags.
- Cross compile Python library for macOS ARM64.
- Migrate to rb-sys and magnus for Ruby library, which adds support for up to Ruby 3.2 and more platforms.
- Cross compile Node.js library for macOS ARM64.
- Use optional dependencies instead of downloading from remote server for Node.js library.

## 0.11.2

- Build and release for Python 3.12.
- Restructure project to use top-level Cargo workspace instead of separate isolated crates.
- Extract out common Rust code to separate published shared crate instead of symlinking.
- Port `gen` code to `build.rs` in common Rust library to avoid requiring Node.js in order to build, and to ensure code stays in sync.
- Rename library folders to `minify-html-*` to better distinguish them from other assorted project code.

## 0.11.1

- Bump GitHub Actions Ubuntu image version.

## 0.11.0

- Change the default CSS minifier optimisation level to 1, as higher levels may perform dangerous optimisations.
- Allow configuring the CSS minifier optimisation level.
- Fix building from source in Node.js postinstall.js script.

## 0.10.8

- [Node.js] Fix assertion failure panic on invalid argument type.
- Do not consider empty `href` attributes as redundant.

## 0.10.7

- Bump [minify-js](https://github.com/wilsonzlin/minify-js) to 0.4.2.

## 0.10.6

- Improve handling of RCDATA text content in edge cases.

## 0.10.5

- Do not encode entities in RCDATA text content (e.g. contents of `<textarea>` and `<title>`).

## 0.10.4

- Use FxHasher for internal hash-based data structures.
- Bump [css-minify](https://github.com/Mnwa/css-minify) to 0.3.1.
- [WASM] Add `type` and `main` fields to package.json.
- [Node.js] Improve invalid argument type error messages.

## 0.10.3

- [Python] Add Python 3.11 support.
- [Python] Build source distribution wheels that will compile on install when prebuilt variants are not available. The Rust compiler must be available.

## 0.10.2

- Bump [minify-js](https://github.com/wilsonzlin/minify-js) to 0.2.6.

## 0.10.1

- Bump [minify-js](https://github.com/wilsonzlin/minify-js) to 0.2.
- Minify JS as module instead of global script if `type` is `module`.

## 0.10.0

- Drop unmatched closing tags instead of reinterpreting them as opening tags. This avoids the possibility of unintentionally creating a large deep tree due to malformed inputs where there are repeated unmatched closing tags (e.g. broken HTML template).
- Fix parallel minification in CLI mode, where some inputs were ignored.
- Output file names as they're processed in parallel mode from the CLI.
- Allow self-closing `<svg>` tags.
- Drop support for macOS ARM64 due to lack of GitHub Actions runners.

## 0.9.2

- Fix Node.js dependency version.
- Create onepass variant for Python.
- Bump [minify-js](https://github.com/wilsonzlin/minify-js) to 0.1.1.
- Implement parallel in-place minification for CLI.

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
