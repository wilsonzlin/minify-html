# hyperbuild

A fast one-pass in-place HTML minifier written in Rust with advanced whitespace handling.

## Features

- Minification is done in one pass with no backtracking or DOM/AST building.
- No extra heap memory is allocated during processing, which increases performance.
- Context-aware whitespace handling allows maximum minification while retaining wanted spaces.

## Usage

```bash
hyperbuild --src /path/to/src.html --out /path/to/output.min.html
```

## Minification

### Whitespace

hyperbuild has advanced whitespace minification that can allow strategies such as:

- Leave whitespace untouched in `pre` and `code`, which are whitespace sensitive.
- Trim and collapse whitespace in content tags, as whitespace is collapsed anyway when rendered.
- Remove whitespace in layout tags, which allows the use of inline layouts while keeping formatted code.

#### Collapsing whitespace

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

#### Destroying whole whitespace

Remove any text nodes that only consist of whitespace characters.

Especially useful when using `display: inline-block` so that whitespace between elements (e.g. indentation) does not alter layout and styling.

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
<ul><li>A</li><li>B</li><li>C</li></ul>
```

</table>

#### Trimming whitespace

Remove any whitespace from the start and end of a tag, if the first and/or last node is a text node.

Useful when combined with whitespace collapsing.

Other whitespace between text nodes and tags are not removed, as it is not recommended to mix non-formatting tags with raw text.

Basically, a tag should only either contain text and [formatting tags](#formatting-tags), or only non-formatting tags.

<table><thead><tr><th>Before<th>After<tbody><tr><td>

```html
<p>↵
··Hey,·I·<em>just</em>·found↵
··out·about·this·<strong>cool</strong>·website!↵
··<div></div>↵
</p>
```

<td>

```html
<p>Hey,·I·<em>just</em>·found↵
··out·about·this·<strong>cool</strong>·website!↵
··<div></div></p>
```

</table>

### Attributes

Any entities in attribute values are decoded, and then the most optimal representation is calculated and used:

- Double quoted, with any `"` encoded.
- Single quoted, with any `'` encoded.
- Unquoted, with `"`/`'` first char (if applicable) and `>` last char (if applicable), and any whitespace, encoded.

Some attributes have their whitespace (after decoding) trimmed and collapsed, such as `class`.

If the attribute value is empty after any processing, it is completely removed (i.e. no `=`).

Spaces are removed between attributes if possible.

### Other

- Comments are removed.
- Entities are decoded if valid (see relevant parsing section).

### Ignored

Empty elements and bangs are not removed as it is assumed there is a special reason for their declaration.

## Parsing

Only UTF-8/ASCII is supported.

hyperbuild is an HTML minifier and simply does HTML minification. hyperbuild almost does no syntax checking or standards enforcement for performance and code complexity reasons.

For example, this means that it's not an error to have self-closing tags, declare multiple `<body>` elements, use incorrect attribute names and values, or write something like `<br>alert('');</br>`

However, there are some syntax requirements for speed and sanity reasons.

### Tags

Tag names are case sensitive.

### Entities

Well-formed entities are decoded, including in attribute values.

They are interpreted as characters representing their decoded value. This means that `&#9;` is considered a whitespace character and could be minified.

Malformed entities are interpreted literally as a sequence of characters.

If a named entity is an invalid reference as per the [spec](https://html.spec.whatwg.org/multipage/named-characters.html#named-character-references), it is considered malformed.

Numeric character references that do not reference a valid [Unicode Scalar Value](https://www.unicode.org/glossary/#unicode_scalar_value) are considered malformed.

### Attributes

Backticks (`` ` ``) are not valid quote marks and are not interpreted as such.
However, backticks are valid attribute value quotes in Internet Explorer.

It is an error to place whitespace between `=` and attribute names/values. It is also an error if there is no whitespace before an attribute. For example:

```html
<div class="a"name="1" id = "a"></div>
```

Special handling of some attributes require case sensitive names and values. For example, `class` and `type="text/javascript"`.

### Script and style

`script` and `style` tags must be closed with `</script>` and `</style>` respectively (case sensitive).

Note that the closing tag must not contain any whitespace (e.g. `</script  >`).

[hyperbuild can handle text script content.](./notes/Text%20script%20content.md)

## Development

### More minification options

- Removal of [optional tags](https://html.spec.whatwg.org/multipage/syntax.html#syntax-tag-omission).
- Removal of boolean attribute values.
- Removal of redundant attributes (empty or default value).
- Handling of conditional or special comments.
