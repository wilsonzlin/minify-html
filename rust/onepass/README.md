# minify-html-onepass

An HTML minifier that provides the functionality of [minify-html](https://github.com/wilsonzlin/minify-html) with much more performance, but with stricter parsing and less configurability.

- Uses the same advanced minification strategy.
- Minifies in one pass with zero memory allocations.
- Outputs in place; no copy or buffer required.

## Performance

<img width="600" alt="Chart showing speed of HTML minifiers" src="https://wilsonl.in/minify-html/bench/0.10.7/core/average-speeds.png">

## Usage

The API is different compared to minify-html; refer to per-package documentation for more details.

- <img width="24" src="https://wilsonl.in/minify-html/icon/python.png"> [minify-html-onepass](https://pypi.org/project/minify-html-onepass)
- <img width="24" src="https://wilsonl.in/minify-html/icon/rust.png"> [minify-html-onepass](https://crates.io/crates/minify-html-onepass)

If you don't see your preferred language here and the main library supports it, raise an issue.

## Parsing

In addition to the [minify-html rules](https://github.com/wilsonzlin/minify-html/blob/master/notes/Parsing.md), the onepass variant has additional requirements:

- Opening tags must not be omitted.
- Invalid closing tags are not allowed.
- The document cannot end unexpectedly.
