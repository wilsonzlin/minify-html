# minify-html changelog

## 0.7.1

- Do not remove `alt` tag when empty.

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
