# minify-html

An HTML minifier meticulously optimised for both speed and effectiveness written in Rust.
Comes with native bindings to Node.js, Python, Java, and Ruby.

- Advanced minification strategy beats other minifiers with only one pass.
- Uses zero memory allocations, SIMD searching, direct tries, and lookup tables.
- Well tested with a large test suite and extensive [fuzzing](./fuzz).
- Natively binds to [esbuild](https://github.com/wilsonzlin/esbuild-rs) for super fast JS and CSS minification.

## Performance

Comparison with [html-minfier](https://github.com/kangax/html-minifier) and [minimize](https://github.com/Swaagie/minimize), run on the top web pages. [See the breakdown here.](./bench)

<img width="415" alt="Chart showing speed of HTML minifiers" src="https://wilsonl.in/minify-html/bench/0.4.3/core/average-speeds.png"> <img width="415" alt="Chart showing effectiveness of HTML minifiers" src="https://wilsonl.in/minify-html/bench/0.4.3/core/average-sizes.png">

## Usage

### CLI

Precompiled binaries are available for x86-64 Linux, macOS, and Windows.

##### Get

[Linux](https://wilsonl.in/minify-html/bin/0.4.3-linux-x86_64) |
[macOS](https://wilsonl.in/minify-html/bin/0.4.3-macos-x86_64) |
[Windows](https://wilsonl.in/minify-html/bin/0.4.3-windows-x86_64.exe)

##### Use

Use the `--help` argument for more details.

```bash
minify-html --src /path/to/src.html --out /path/to/output.min.html --css --js
```

### API

<details>
<summary><strong>Rust</strong></summary>

##### Get

```toml
[dependencies]
minify-html = { version = "0.4.3", features = ["js-esbuild"] }
```

Building with the `js-esbuild` feature requires the Go compiler to be installed as well, to build the [JS and CSS minifier](https://github.com/wilsonzlin/esbuild-rs).

If the `js-esbuild` feature is not enabled, `cfg.minify_js` and `cfg.minify_css` will have no effect.

##### Use

Check out the [docs](https://docs.rs/minify-html) for API and usage examples.

</details>

<details>
<summary><strong>Node.js</strong></summary>

- Package: [@minify-html/js](https://www.npmjs.com/package/@minify-html/js)
- Binding: [N-API](https://nodejs.org/api/n-api.html)
- Platforms: Linux, macOS, Windows; Node.js 8.6.0 and higher

##### Get

Using npm:

```bash
npm i @minify-html/js
```

Using Yarn:

```bash
yarn add @minify-html/js
```

##### Use

```js
const minifyHtml = require("@minify-html/js");

const cfg = minifyHtml.createConfiguration({ minifyJs: false, minifyCss: false });
const minified = minifyHtml.minify("<p>  Hello, world!  </p>", cfg);

// Alternatively, minify in place to avoid copying.
const source = Buffer.from("<p>  Hello, world!  </p>");
// This is a Buffer representing a slice of `source`, not newly allocated memory.
const minified = minifyHtml.minifyInPlace(source, cfg);
```

minify-html is also available for TypeScript:

```ts
import * as minifyHtml from "@minify-html/js";
import * as fs from "fs";

const cfg = minifyHtml.createConfiguration({ minifyJs: false, minifyCss: false });
const minified = minifyHtml.minify("<p>  Hello, world!  </p>", cfg);
// Or alternatively:
const minified = minifyHtml.minifyInPlace(fs.readFileSync("source.html"), cfg);
```

</details>

<details>
<summary><strong>Java</strong></summary>

- Package: [in.wilsonl.minifyhtml](https://search.maven.org/artifact/in.wilsonl.minifyhtml/minify-html)
- Binding: [JNI](https://github.com/jni-rs/jni-rs)
- Platforms: Linux, macOS, Windows; Java 7 and higher

##### Get

Add as a Maven dependency:

```xml
<dependency>
  <groupId>in.wilsonl.minifyhtml</groupId>
  <artifactId>minify-html</artifactId>
  <version>0.4.3</version>
</dependency>
```

##### Use

```java
import in.wilsonl.minifyhtml.Configuration;
import in.wilsonl.minifyhtml.MinifyHtml;
import in.wilsonl.minifyhtml.SyntaxException;

Configuration cfg = new Configuration.Builder()
    .setMinifyJs(false)
    .setMinifyCss(false)
    .build();

try {
    String minified = MinifyHtml.minify("<p>  Hello, world!  </p>", cfg);
} catch (SyntaxException e) {
    System.err.println(e.getMessage());
}

// Alternatively, minify in place:
assert source instanceof ByteBuffer && source.isDirect();
MinifyHtml.minifyInPlace(source, cfg);
```

</details>

<details>
<summary><strong>Python</strong></summary>

- Package: [minify-html](https://pypi.org/project/minify-html)
- Binding: [PyO3](https://github.com/PyO3/pyo3)
- Platforms: Linux, macOS, Windows; Python 3.7 and higher

##### Get

Add the PyPI project as a dependency and install it using `pip` or `pipenv`.

##### Use

```python
import minify_html

try:
    minified = minify_html.minify("<p>  Hello, world!  </p>", minify_js=False, minify_css=False)
except SyntaxError as e:
    print(e)
```

</details>

<details>
<summary><strong>Ruby</strong></summary>

- Package: [minify_html](https://rubygems.org/gems/minify_html)
- Binding: [Rutie](https://github.com/danielpclark/rutie)
- Platforms: Linux, macOS; Ruby 2.5 and higher

##### Get

Add the library as a dependency to `Gemfile` or `*.gemspec`.

##### Use

```ruby
require 'minify_html'

print MinifyHtml.minify("<p>  Hello, world!  </p>", { :minify_js => false, :minify_css => false })
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

Entities are decoded if they're valid and shorter or equal in length when decoded.

Numeric entities that do not refer to a valid [Unicode Scalar Value](https://www.unicode.org/glossary/#unicode_scalar_value) are replaced with the [replacement character](https://en.wikipedia.org/wiki/Specials_(Unicode_block)#Replacement_character).

If an entity is unintentionally formed after decoding, the leading ampersand is encoded, e.g. `&&#97;&#109;&#112;;` becomes `&ampamp;`. This is done as `&amp` is equal to or shorter than all other entity representations of characters part of an entity (`[&#a-zA-Z0-9;]`), and there is no other conflicting entity name that starts with `amp`.

Note that it's possible to get an unintentional entity after removing comments, e.g. `&am<!-- -->p`; minify-html will **not** encode the leading ampersand.

### Comments

Comments are removed.

### Ignored

Bangs, [processing instructions](https://en.wikipedia.org/wiki/Processing_Instruction), and empty elements are not removed as it is assumed there is a special reason for their declaration.

## Parsing

- Input must be UTF-8.
- Opening tags must not be [omitted](https://html.spec.whatwg.org/multipage/syntax.html#syntax-tag-omission).
- [Escaped and double-escaped](./notes/Script%20data.md) script content are not supported.

## Issues and contributions

Pull requests and any contributions welcome!

If minify-html did something unexpected, misunderstood some syntax, or incorrectly kept/removed some code, [raise an issue](https://github.com/wilsonzlin/minify-html/issues) with some relevant code that can be used to reproduce and investigate the issue.
