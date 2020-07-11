# minify-html

A fast one-pass in-place HTML minifier written in Rust with context-aware whitespace handling.

Also supports JS minification by plugging into [esbuild](https://github.com/evanw/esbuild).

Available as:
- CLI for macOS and Linux.
- Rust library.
- Native library for Node.js, Python, Java, and Ruby.

## Features

- Minification is done in one pass with no backtracking or DOM/AST building.
- No extra heap memory is allocated during processing, which increases performance.
- Context-aware whitespace handling allows maximum minification while retaining desired spaces.
- Well tested with a large test suite and extensive [fuzzing](./fuzz).

## Performance

Speed and effectiveness of Node.js version compared to [html-minfier](https://github.com/kangax/html-minifier) and [minimize](https://github.com/Swaagie/minimize), run on popular already-minified web pages. See [bench](./bench) folder for more details.

<img width="435" alt="Chart showing speed of HTML minifiers" src="https://wilsonl.in/minify-html/bench/0.2.6/average-speeds.png"> <img width="435" alt="Chart showing effectiveness of HTML minifiers" src="https://wilsonl.in/minify-html/bench/0.2.6/average-sizes.png">

## Usage

### CLI

Precompiled binaries are available for x86-64 macOS and Linux.

##### Get

[macOS](https://wilsonl.in/minify-html/bin/0.2.6-macos-x86_64) |
[Linux](https://wilsonl.in/minify-html/bin/0.2.6-linux-x86_64)

##### Use

Use the `--help` argument for more details.

```bash
minify-html --src /path/to/src.html --out /path/to/output.min.html
```

### API

<details>
<summary><strong>Rust</strong></summary>

##### Get

```toml
[dependencies]
minify-html = { version = "0.2.6", features = ["js-esbuild"] }
```

Building with the `js-esbuild` feature requires the Go compiler to be installed as well, to build the [JS minifier](https://github.com/evanw/esbuild).

If the `js-esbuild` feature is not enabled, `cfg.minify_js` will have no effect.

##### Use

```rust
use minify_html::{Cfg, FriendlyError, in_place, copy, with_friendly_error, truncate};

fn main() {
    let mut code = b"<p>  Hello, world!  </p>".to_vec();
    let cfg = &Cfg {
        minify_js: false,
    };

    // Minifies a slice in-place and returns the new minified length,
    // but leaves any original code after the minified code intact.
    match in_place(&mut code, cfg) {
        Ok(minified_len) => {}
        Err((error_type, error_position)) => {}
    };

    // Creates a vector copy containing only minified code
    // instead of minifying in-place.
    match copy(&code, cfg) {
        Ok(minified) => {}
        Err((error_type, error_position)) => {}
    };

    // Minifies a vector in-place, and then truncates the
    // vector to the new minified length.
    match truncate(&mut code, cfg) {
        Ok(()) => {}
        Err((error_type, error_position)) => {}
    };

    // Identical to `in_place` except with FriendlyError instead.
    // `code_context` is a string of a visual representation of the source,
    // with line numbers and position markers to aid in debugging syntax.
    match with_friendly_error(&mut code, cfg) {
        Ok(minified_len) => {}
        Err(FriendlyError { position, message, code_context }) => {
            eprintln!("Failed at character {}:", position);
            eprintln!("{}", message);
            eprintln!("{}", code_context);
        }
    };
}
```

</details>

<details>
<summary><strong>Node.js</strong></summary>

Package: [@minify-html/js-esbuild](https://www.npmjs.com/package/@minify-html/js-esbuild),
Binding: [Neon](https://neon-bindings.com/)
Platforms: macOS, Linux; Node.js 8 and higher

##### Get

Using npm:

```bash
npm i @minify-html/js-esbuild
```

Using Yarn:

```bash
yarn add @minify-html/js-esbuild
```

##### Use

```js
const minifyHtml = require("@minify-html/js-esbuild");

const cfg = { minifyJs: false };
const minified = minifyHtml.minify("<p>  Hello, world!  </p>", cfg);

// Alternatively, minify in place to avoid copying.
const source = Buffer.from("<p>  Hello, world!  </p>", cfg);
minifyHtml.minifyInPlace(source);
```

minify-html is also available for TypeScript:

```ts
import * as minifyHtml from "@minify-html/js-esbuild";
import * as fs from "fs";

const cfg = { minifyJs: false };
const minified = minifyHtml.minify("<p>  Hello, world!  </p>", cfg);
minifyHtml.minifyInPlace(fs.readFileSync("source.html"), cfg);
```

</details>

<details>
<summary><strong>Java</strong></summary>

Package: [in.wilsonl.minifyhtml](https://search.maven.org/artifact/in.wilsonl.minifyhtml/minify-html)
Binding: [JNI](https://github.com/jni-rs/jni-rs)
Platforms: macOS, Linux; Java 7 and higher

##### Get

Add as a Maven dependency:

```xml
<dependency>
  <groupId>in.wilsonl.minifyhtml</groupId>
  <artifactId>minify-html</artifactId>
  <version>0.2.6</version>
</dependency>
```

##### Use

```java
import in.wilsonl.minify_html.MinifyHtml;

MinifyHtml.Configuration cfg = new MinifyHtml.Configuration.Builder()
    .setMinifyJs(false)
    .build();
try {
    String minified = MinifyHtml.minify("<p>  Hello, world!  </p>", cfg);
} catch (MinifyHtml.SyntaxException e) {
    System.err.println(e.getMessage());
}

// Alternatively, minify in place:
assert source instanceof ByteBuffer && source.isDirect();
MinifyHtml.minifyInPlace(source, cfg);
```

</details>

<details>
<summary><strong>Python</strong></summary>

Package: [minify_html](https://pypi.org/project/minify_html)
Binding: [PyO3](https://github.com/PyO3/pyo3)
Platforms: macOS, Linux; CPython 3.5 and higher

##### Get

Add the PyPI project as a dependency and install it using `pip` or `pipenv`.

##### Use

```python
import minify_html

try:
    minified = minify_html.minify("<p>  Hello, world!  </p>", minify_js=False)
except SyntaxError as e:
    print(e)
```

</details>

<details>
<summary><strong>Ruby</strong></summary>

Package: [minify_html](https://rubygems.org/gems/minify_html)
Binding: [Rutie](https://github.com/danielpclark/rutie)
Platforms: macOS, Linux; Ruby 2.5 and higher

##### Get

Add the library as a dependency to `Gemfile` or `*.gemspec`.

##### Use

```ruby
require 'minify_html'

print MinifyHtml.minify("<p>  Hello, world!  </p>", { :minify_js => false })
```

</details>

## Minification

### Whitespace

minify-html has advanced context-aware whitespace minification that does things such as:

- Leave whitespace untouched in `pre` and `code`, which are whitespace sensitive.
- Trim and collapse whitespace in content tags, as whitespace is collapsed anyway when rendered.
- Remove whitespace in layout tags, which allows the use of inline layouts while keeping formatted code.

#### Methods

There are three whitespace minification methods. When processing text content, minify-html chooses which ones to use depending on the containing element.

<details>
<summary><strong>Collapse whitespace</strong></summary>

> **Applies to:** any element except [whitespace sensitive](./src/spec/tag/whitespace.rs) elements.

Reduce a sequence of whitespace characters in text nodes to a single space (U+0020).

<table><thead><tr><th>Before<th>After<tbody><tr><td>

```html
<p>↵
··The·quick·brown·fox↵
··jumps·over·the·lazy↵
··dog.↵
</p>
```

<td>

```html
<p>·The·quick·brown·fox·jumps·over·the·lazy·dog.·</p>
```

</table>
</details>

<details>
<summary><strong>Destroy whole whitespace</strong></summary>

> **Applies to:** any element except [whitespace sensitive](./src/spec/tag/whitespace.rs), [content](src/spec/tag/whitespace.rs), [content-first](./src/spec/tag/whitespace.rs), and [formatting](./src/spec/tag/whitespace.rs) elements.

Remove any text nodes between tags that only consist of whitespace characters.

<table><thead><tr><th>Before<th>After<tbody><tr><td>

```html
<ul>↵
··<li>A</li>↵
··<li>B</li>↵
··<li>C</li>↵
</ul>
```

<td>

```html
<ul>↵
··<li>A</li><li>B</li><li>C</li>↵
</ul>
```

</table>
</details>

<details>
<summary><strong>Trim whitespace</strong></summary>

> **Applies to:** any element except [whitespace sensitive](./src/spec/tag/whitespace.rs) and [formatting](./src/spec/tag/whitespace.rs) elements.

Remove any leading/trailing whitespace from any leading/trailing text nodes of a tag.

<table><thead><tr><th>Before<th>After<tbody><tr><td>

```html
<p>↵
··Hey,·I·<em>just</em>·found↵
··out·about·this·<strong>cool</strong>·website!↵
··<sup>[1]</sup>↵
</p>
```

<td>

```html
<p>Hey,·I·<em>just</em>·found↵
··out·about·this·<strong>cool</strong>·website!↵
··<sup>[1]</sup></p>
```

</table>
</details>

#### Element types

minify-html recognises elements based on one of a few ways it assumes they are used. By making these assumptions, it can apply optimal whitespace minification strategies.

|Group|Elements|Expected children|
|---|---|---|
|Formatting|`a`, `strong`, [and others](./src/spec/tag/whitespace.rs)|Formatting elements, text.|
|Content|`h1`, `p`, [and others](src/spec/tag/whitespace.rs)|Formatting elements, text.|
|Layout|`div`, `ul`, [and others](./src/spec/tag/whitespace.rs)|Layout elements, content elements.|
|Content-first|`label`, `li`, [and others](./src/spec/tag/whitespace.rs)|Like content but could be layout with only one child.|

<details>
<summary><strong>Formatting elements</strong></summary>

> Whitespace is collapsed.

Formatting elements are usually inline elements that wrap around part of some text in a content element, so its whitespace isn't trimmed as they're probably part of the content.

</details>

<details>
<summary><strong>Content elements</strong></summary>

> Whitespace is trimmed and collapsed.

Content elements usually represent a contiguous and complete unit of content such as a paragraph. As such, whitespace is significant but sequences of them are most likely due to formatting.

###### Before

```html
<p>↵
··Hey,·I·<em>just</em>·found↵
··out·about·this·<strong>cool</strong>·website!↵
··<sup>[1]</sup>↵
</p>
```

###### After

```html
<p>Hey,·I·<em>just</em>·found·out·about·this·<strong>cool</strong>·website!·<sup>[1]</sup></p>
```

</details>

<details>
<summary><strong>Layout elements</strong></summary>

> Whitespace is trimmed and collapsed. Whole whitespace is removed.

These elements should only contain other elements and no text. This makes it possible to remove whole whitespace, which is useful when using `display: inline-block` so that whitespace between elements (e.g. indentation) does not alter layout and styling.

###### Before

```html
<ul>↵
··<li>A</li>↵
··<li>B</li>↵
··<li>C</li>↵
</ul>
```

###### After

```html
<ul><li>A</li><li>B</li><li>C</li></ul>
```

</details>

<details>
<summary><strong>Content-first elements</strong></summary>

> Whitespace is trimmed and collapsed.

These elements are usually like content elements but are occasionally used like a layout element with one child. Whole whitespace is not removed as it might contain content, but this is OK for using as layout as there is only one child and whitespace is trimmed.

###### Before

```html
<li>↵
··<article>↵
····<section></section>↵
····<section></section>↵
··</article>↵
</li>
```

###### After

```html
<li><article><section></section><section></section></article></li>
```

</details>

### Tags

[Optional closing tags](https://html.spec.whatwg.org/multipage/syntax.html#syntax-tag-omission) are removed.

### Attributes

Any entities in attribute values are decoded, and then the shortest representation of the value is calculated and used:

- Double quoted, with any `"` encoded.
- Single quoted, with any `'` encoded.
- Unquoted, with `"`/`'` first character (if applicable), `>` last character (if applicable), and any whitespace encoded.

`class` and `d` attributes have their whitespace (after any decoding) trimmed and collapsed.

[Boolean attribute](./gen/attrs.json) values are removed.
[Some other attributes](./gen/attrs.json) are completely removed if their value is empty or the default value after any processing.

`type` attributes on `script` tags with a value equaling a [JavaScript MIME type](https://mimesniff.spec.whatwg.org/#javascript-mime-type) are removed.

If an attribute value is empty after any processing, everything but the name is completely removed (i.e. no `=`), as an empty attribute is implicitly [the same](https://html.spec.whatwg.org/multipage/syntax.html#attributes-2) as an attribute with an empty string value.

Spaces are removed between attributes if possible.

### Entities

Entities are decoded if valid (see relevant parsing section) and their decoded characters as UTF-8 is shorter or equal in length.

Numeric entities that do not refer to a valid [Unicode Scalar Value](https://www.unicode.org/glossary/#unicode_scalar_value) are replaced with the [replacement character](https://en.wikipedia.org/wiki/Specials_(Unicode_block)#Replacement_character).

If an entity is unintentionally formed after decoding, the leading ampersand is encoded, e.g. `&&#97;&#109;&#112;;` becomes `&ampamp;`. This is done as `&amp` is equal to or shorter than all other entity representations of characters part of an entity (`[&#a-zA-Z0-9;]`), and there is no other conflicting entity name that starts with `amp`.

It's possible to get an unintentional entity after removing comments, e.g. `&am<!-- -->p`.

Left chevrons after any decoding in text are encoded to `&LT` if possible or `&LT;` otherwise.

### Comments

Comments are removed.

### Ignored

Bangs, [processing instructions](https://en.wikipedia.org/wiki/Processing_Instruction), and empty elements are not removed as it is assumed there is a special reason for their declaration.

## Parsing

Only UTF-8/ASCII-encoded HTML code is supported.

minify-html does no syntax checking or standards enforcement for performance and code complexity reasons.

For example, this means that it's not an error to have self-closing tags, declare multiple `<body>` elements, use incorrect attribute names and values, or write something like `<br>alert('');</br>`

However, there are some syntax requirements for speed and sanity.

### Tags

Tag names are case sensitive. For example, this means that `P` won't be recognised as a content element, `bR` won't be considered as a void tag, and the contents of `Script` won't be parsed as JavaScript.

Tags must not be [omitted](https://html.spec.whatwg.org/multipage/syntax.html#syntax-tag-omission). Void tags must not have a separate closing tag e.g. `</input>`.

### Entities

Well-formed entities are decoded, including in attribute values.

They are interpreted as characters representing their decoded value. This means that `&#9;` is considered a whitespace character and could be minified.

Malformed entities are interpreted literally as a sequence of characters.

If a named entity is an invalid reference as per the [specification](https://html.spec.whatwg.org/multipage/named-characters.html#named-character-references), it is considered malformed.

Numeric character references that do not reference a valid [Unicode Scalar Value](https://www.unicode.org/glossary/#unicode_scalar_value) are considered malformed.

### Attributes

Backticks (`` ` ``) are not valid quote marks and not interpreted as such.
However, backticks are valid attribute value quotes in Internet Explorer.

Special handling of some attributes require case sensitive names and values. For example, `CLASS` won't be recognised as an attribute to minify, and `type="Text/JavaScript"` on a `<script>` will not be removed.

### Script and style

`script` and `style` tags must be closed with `</script>` and `</style>` respectively (case sensitive).

minify-html does **not** handle [escaped and double-escaped](./notes/Script%20data.md) script content.

## Issues and contributions

Pull requests and any contributions welcome!

If minify-html did something unexpected, misunderstood some syntax, or incorrectly kept/removed some code, [raise an issue](https://github.com/wilsonzlin/minify-html/issues) with some relevant code that can be used to reproduce and investigate the issue.
