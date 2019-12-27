# hyperbuild

A fast one-pass in-place HTML minifier written in Rust with advanced whitespace handling.

Currently in beta, working on documentation and tests. Issues and pull requests welcome! Guide below is currently WIP.

## Features

- Minification is done in one pass with no backtracking or DOM/AST building.
- No dynamic memory is allocated during processing, which increases safety and performance.
- Advanced whitespace handling options allow maximum minification while retaining valid spaces.

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

### WIP

- Removal of [optional tags](https://html.spec.whatwg.org/multipage/syntax.html#syntax-tag-omission).
- Removal of boolean attribute values.
- Removal of redundant attributes (empty or default value).
- Handling of conditional or special comments.

### Explicitly important

Empty elements and bangs are not removed as it is assumed there is a special reason for their declaration.

## Parsing

hyperbuild is an HTML minifier and simply does HTML minification. In addition to keeping to one role, hyperbuild almost does no syntax checking or standards enforcement for performance and code complexity reasons.

For example, this means that it's not an error to have self-closing tags, declare multiple `<body>` elements, use incorrect attribute names and values, or write something like `<br>alert('');</br>`

However, there are some syntax requirements for speed and sanity reasons.

### Tags

Tag names are case sensitive.

### Entities  

Well-formed entities are decoded, including in attribute values. 
 
They are considered as a single character representing their decoded value. This means that `&#9;` is considered a whitespace character and could be minified.

If a named entity is an invalid reference as per the [spec](https://html.spec.whatwg.org/multipage/named-characters.html#named-character-references), it is considered malformed and will be interpreted literally.

Numeric character references that do not reference a valid [Unicode Scalar Value](https://www.unicode.org/glossary/#unicode_scalar_value) are considered malformed.

### Attributes

Backticks (`` ` ``) are not valid quote marks and are not interpreted as such.
However, backticks are valid attribute value quotes in Internet Explorer.

It's an error to place whitespace between `=` and attribute names/values.

Special handling of some attributes require case-sensitive names and values. For example, `class` and `type="text/javascript"`.

It's an error if there is no whitespace before an attribute.

Most likely, the cause of this error is either invalid syntax or something like:

```html
<div class="a"name="1"></div>
```

(Note the lack of space between the end of the `class` attribute and the beginning of the `name` attribute.)

### Script and style

`script` and `style` tags must be closed with `</script>` and `</style>` respectively (case sensitive).

Note that the closing tag must not contain any whitespace (e.g. `</script  >`).

[hyperbuild can handle text script content.](notes/Text script content.md)

## Development

Currently, hyperbuild has a few limitations:

- Only UTF-8/ASCII is supported.
- Not aware of exotic Unicode whitespace characters.

Patches to change any of these welcome!
