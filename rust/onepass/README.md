# minify-html-onepass

An HTML minifier that provides the functionality of [minify-html](https://github.com/wilsonzlin/minify-html) with much more performance, but with stricter parsing and less configurability.

- Uses the same advanced minification strategy.
- Minifies in one pass with zero memory allocations.
- Outputs in place; no copy or buffer required.

## Performance

<img width="600" alt="Chart showing speed of HTML minifiers" src="https://wilsonl.in/minify-html/bench/0.8.1/core/average-speeds.png">

## Usage

The API is different compared to minify-html; refer to per-package documentation for more details.

- <img width="24" src="https://wilsonl.in/minify-html/icon/java.png"> [in.wilsonl.minifyhtmlonepass](https://search.maven.org/artifact/in.wilsonl.minifyhtmlonepass/minify-html-onepass)
- <img width="24" src="https://wilsonl.in/minify-html/icon/nodejs.png"> [@minify-html/onepass](https://www.npmjs.com/package/@minify-html/onepass)
- <img width="24" src="https://wilsonl.in/minify-html/icon/python.png"> [minify-html-onepass](https://pypi.org/project/minify-html-onepass)
- <img width="24" src="https://wilsonl.in/minify-html/icon/ruby.png"> [minify_html_onepass](https://rubygems.org/gems/minify_html_onepass)
- <img width="24" src="https://wilsonl.in/minify-html/icon/rust.png"> [minify-html-onepass](https://crates.io/crates/minify-html-onepass)

## Parsing

In addition to the [minify-html rules](https://github.com/wilsonzlin/minify-html/blob/master/notes/Parsing.md), the onepass variant has additional requirements:

- Opening tags must not be omitted.
- Invalid closing tags are not allowed.
- The document cannot end unexpectedly.
