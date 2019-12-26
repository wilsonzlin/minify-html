# hyperbuild

A fast one-pass in-place HTML minifier written in Rust with advanced whitespace handling.

Currently in beta, working on documentation and tests. Issues and pull requests welcome!

## Features

- Minification is done in one pass with no backtracking or DOM/AST building.
- No dynamic memory is allocated during processing, which increases safety and performance.
- Advanced whitespace handling options allow maximum minification while retaining valid spaces.

## Usage

```bash
hyperbuild --src /path/to/src.html --out /path/to/output.min.html
```

## Minification

Guide below is currently WIP.

### Whitespace

hyperbuild has advanced whitespace minification that can allow strategies such as:

- leave whitespace untouched in `pre` and `code`, which are whitespace sensitive
- trim and collapse whitespace in content tags, as whitespace is collapsed anyway when rendered
- remove whitespace in layout tags, which allows the use of inline layouts while keeping formatted code

## Parsing

hyperbuild is an HTML minifier and simply does HTML minification. In addition to keeping to one role, hyperbuild almost does no syntax checking or standards enforcement for performance and code complexity reasons.

For example, this means that it's not an error to have self-closing tags, having multiple `<body>` elements, using incorrect attribute names and values, or using `<br>` like `<br>alert('');</br>`

However, there are some syntax requirements for speed and sanity reasons.

### Tags

Tag names are case sensitive.

### Entities  

Well-formed entities are decoded, including in attribute values. 
 
They are considered as a single character representing their decoded value. This means that `&#9;` is considered a whitespace character and could be minified.

If a named entity is an invalid reference as per the [spec](https://html.spec.whatwg.org/multipage/named-characters.html#named-character-references), it is considered malformed and will be interpreted literally.

Numeric character references that reference to numbers below 0x00 or above 0x10FFFF are considered malformed. It will be decoded if it falls within this range, even if it does not refer to a valid Unicode code point.

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

`script` and `style` tags must be closed with `</script>` and `</style>` respectively (case-sensitive).

Note that the closing tag must not contain any whitespace (e.g. `</script  >`).

## Development

Currently, hyperbuild has a few limitations:

- Only UTF-8/ASCII is supported.
- Not aware of exotic Unicode whitespace characters.

Patches to change any of these welcome!
