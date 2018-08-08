# hyperbuild

A fast HTML parser, preprocessor, and minifier, written in C.
Designed to be used in C projects, but also runnable on Node.js.
Minifier heavily influenced by [kangax's html-minifier](https://github.com/kangax/html-minifier).

## Features

### Streaming minification

hyperbuild minifies as it parses, directly streaming processed HTML to the output without having to build a DOM/AST or iterate/traverse around in multiple passes, allowing for super-fast compilation times and near-constant memory usage.

### Smart parsing

hyperbuild is aware of strings and comments in JS and CSS sections, and deals with them correctly.

### Super low level

hyperbuild is written in C, and has a small Node.js wrapper module that simplifies calling the executable.

### Special precise whitespace handling

hyperbuild can minify whitespace in different ways and depending on the surrounding context, according to precise and advanced rules.

## Parsing

Current limitations:

- UTF-8 in, UTF-8 out, no BOM at any time.
- Not aware of exotic Unicode whitespace characters.
- Tested and designed for Linux only.
- Follows HTML5 only.

### Errors

Errors marked with a `⌫` can be suppressed using the [`--suppress`](#--suppress) option.
Use the error name without the `HBE_PARSE_` prefix.

#### `HBE_PARSE_MALFORMED_ENTITY` ⌫

It's an error if the sequence of characters following an ampersand (`&`) does not form a valid entity.

Entities must be of one of the following forms:

- `&name;`, where *name* is a reference to a valid HTML entity
- `&nnnn;`, where *nnnn* is a Unicode code point in base 10
- `&#xhhhh;`, where *hhhh* is a Unicode code point in base 16

A malformed entity is an ampersand not followed by a sequence of characters that matches one of the above forms. This includes when the semicolon is missing.

Note that this is different from `HBE_PARSE_INVALID_ENTITY`, which is when a well-formed entity references a non-existent entity name or Unicode code point.

While an ampersand by itself (i.e. followed by whitespace or as the last character) is a malformed entity, it is covered by `HBE_PARSE_BARE_AMPERSAND`.

#### `HBE_PARSE_BARE_AMPERSAND` ⌫

It's an error to have an ampersand followed by whitespace or as the last character.

This is intentionally a different error to `HBE_PARSE_MALFORMED_ENTITY` due to the ubiquity of bare ampersands.

An ampersand by itself is not *necessarily* an invalid entity. However, HTML parsers and browsers may have different interpretations of bare ampersands, so it's a good idea to always use the encoded form (`&amp;`).

When this error is suppressed, bare ampersands are outputted untouched.

#### `HBE_PARSE_INVALID_ENTITY` ⌫

It's an error if an invalid HTML entity is detected.

If suppressed, invalid entities are outputted untouched.

See [entityrefs.c](src/main/c/rule/entity/entityrefs.c) for the list of entity references considered valid by hyperbuild.

Valid entities that reference a Unicode code point must be between 0x0 and 0x10FFFF (inclusive).

#### `HBE_PARSE_NONSTANDARD_TAG` ⌫

It's an error if an unknown (non-standard) tag is reached.
See [tags.c](src/main/c/rule/tag/tags.c) for the list of tags considered valid by hyperbuild.

#### `HBE_PARSE_UCASE_TAG` ⌫

It's an error if an opening or closing tag's name has any uppercase characters.

#### `HBE_PARSE_UCASE_ATTR` ⌫

It's an error if an attribute's name has any uppercase characters.

#### `HBE_PARSE_UNQUOTED_ATTR` ⌫

It's an error if an attribute's value is not quoted with `"` (U+0022) or `'` (U+0027).
This means that `` ` `` is not a valid quote mark regardless of whether this error is suppressed or not. Backticks are valid attribute value quotes in Internet Explorer.

#### `HBE_PARSE_ILLEGAL_CHILD`

It's an error if a tag is declared where it can't be a child of.
This is a very simple check, and does not cover the comprehensive HTML rules, which involve backtracking, tree traversal, and lots of conditionals.

This rule is enforced in four parts:
[whitelistparents.c](src/main/c/rule/relation/whitelistparents.c),
[blacklistparents.c](src/main/c/rule/relation/blacklistparents.c),
[whitelistchildren.c](src/main/c/rule/relation/whitelistchildren.c), and
[blacklistchildren.c](src/main/c/rule/relation/blacklistchildren.c).

#### `HBE_PARSE_UNCLOSED_TAG`

It's an error if a non-void tag is not closed.
See [voidtags.c](src/main/c/rule/tag/voidtags.c) for the list of tags considered void by hyperbuild.

This includes tags that close automatically because of siblings (e.g. `<li><li>`), as it greatly simplifies the complexity of the minifier due to guarantees about the structure.

#### `HBE_PARSE_SELF_CLOSING_TAG` ⌫

It's an error if a tag is self-closed. Valid in XML, not in HTML.

#### `HBE_PARSE_NO_SPACE_BEFORE_ATTR`

It's an error if there is no whitespace before an attribute.

Most likely, the cause of this error is either invalid syntax or something like:

```html
<div class="a"name="1"></div>
```

(Note the lack of space between the end of the `class` attribute and the beginning of the `name` attribute.)

#### `HBE_PARSE_UNEXPECTED_END` and `HBE_PARSE_EXPECTED_NOT_FOUND`

General syntax errors.

#### Additional errors

There are additional implicit errors that are considered as general syntax errors due to the way the parser works:

- Closing void tags; see [voidtags.c](src/main/c/rule/tag/voidtags.c) for the list of tags considered void by hyperbuild.
- Placing whitespace between `=` and attribute names/values.
- Placing whitespace before the tag name in an opening tag.
- Placing whitespace around the tag name in a closing tag.
- Not closing a tag before the end of the file/input.

#### Notes

- Closing `</script>` tags end single-line and multi-line JavaScript comments in `script` tags.
  For this to be detected by hyperbuild, the closing tag must not contain any whitespace (e.g. `</script  >`).

### Options

#### `--in`

Path to a file to process. If omitted, hyperbuild will read from `stdin`, and imports will be relative to the working directory.

#### `--out`

Path to a file to write to; it will be created if it doesn't exist already. If omitted, the output will be streamed to `stdout`.

#### `--keep`

Don't automatically delete the output file if an error occurred. If the output is `stdout`, or the output is a file but `--buffer` is provided, this option does nothing.

#### `--buffer`

Buffer all output until the process is complete and successful. This won't truncate or write anything to the output until the build process is done, but will use a non-constant amount of memory.
This applies even when the output is `stdout`.

#### `--suppress`

Suppress errors specified by this option. hyperbuild will quitely ignore and continue processing when otherwise one of the provided errors would occur.

Omit the `HBE_PARSE_` prefix. Separate the error names with commas. Suppressible errors are marked with a `⌫` in the [Errors](#errors) section.

## Processing

hyperbuild sits somewhere between Server Side Includes and a templating library, and is designed for simplistic compilation of apps statically rather than dynamic generation of live content.

To achieve this, hyperbuild has special **directives** that allow special action to be taken when it's processing some HTML code.
This includes importing files, getting and setting variables, and escaping text for HTML.

Directives are like functions in any common language: they take some arguments, and return some value.
In hyperbuild, all arguments are simple strings, and the return value is directly streamed while processing.

### Using directives

There are two methods of getting hyperbuild's attention: using a special tag, and using a special entity.

#### Directive tags

```html
<hb-dir arg1="val1" arg2="val2">valarg</hb-dir>
```

- Replace `dir` with a hyperbuild directive name
- The value for the argument `value` is provided via the inner content of the tag
- All other arguments are provided via attributes
- Directive entities inside argument values, and nested directive tags, will be processed

#### Directive entities

```html
&hb-dir(arg1=val1, arg2=val2);
```

- Replace `dir` with a hyperbuild directive name
- Arguments are provided in name-value pairs between parentheses, separated by commas
- All characters between the `=` and next `,` or `)` count as the argument's value, including whitespace characters
- To use commas, right parentheses, or ampersands in argument values, use HTML entities (`&#44;`, `&#41;`, and `&amp;`)
- Directive entities inside argument values will be processed

### Available directives

#### `import`

Read, parse, process, and minify another file, and stream the result.

|Argument|Format|Required|Description|
|---|---|---|---|
|path|Relative or absolute file system path|Y|The path to the file. If it starts with a slash, it is interpreted as an absolute path; otherwise, it's a path relative to the directory of the importee, or the working directory if the input is `stdin`.|

## Minification

### Theory

#### Whitespace

##### Beginning and end

```html
<p>↵
··The·quick·brown·fox↵
</p>
```

##### Between text and tags

```html
<p>The·quick·brown·fox·<strong>jumps</strong>·over·the·lazy·dog.</p>
```

##### Contiguous

```html
<select>↵
··<option>Jan:·········1</option>↵
··<option>Feb:········10</option>↵
··<option>Mar:·······100</option>↵
··<option>Apr:······1000</option>↵
··<option>May:·····10000</option>↵
··<option>Jun:····100000</option>↵
</select>
```

##### Whole text

```html
<p>↵
···↵
</p>
```

#### Content

##### Specific tags

Tags not in one of the categories below are **specific tags**.

##### Formatting tags

```html
<strong> moat </strong>
```

##### Content tags

```html
<p>Some <strong>content</strong></p>
```

##### Content-first tags

```html
<li>Anthony</li>
```

```html
<li>
  <div>
  </div>
</li>
```

##### Layout tags

```html
<div>
  <div></div>
</div>
```

##### Overview

|Type|Content|
|---|---|
|Formatting tags|Text nodes|
|Content tags|Formatting tags, text nodes|
|Layout tags|Layout tags, content tags|
|Content-first tags|Content of content tags or layout tags (but not both)|

### Options

Note that only existing whitespace will be up for removal via minification. Entities that represent whitespace will not be decoded and then removed.

For options that have a list of tags as their values, the tags should be separated by a comma.

An `*` (asterisk, U+002A) can be used to represent the complete set of possible tags. It essentially fully enables or disables the option.

For brevity, hyperbuild has built-in sets of tags that can be used in place of declaring all their members; they begin with a `$` sign:

|Name|Tags|Source|
|---|---|---|
|`$content`|`address`, `audio`, `button`, `canvas`, `caption`, `figcaption`, `h1`, `h2`, `h3`, `h4`, `h5`, `h6`, `legend`, `meter`, `object`, `option`, `p`, `summary`, `textarea`, `video`|[contenttags.c](src/main/c/rule/tag/contenttags.c)|
|`$contentfirst`|`dd`, `details`, `dt`, `iframe`, `label`, `li`, `noscript`, `output`, `progress`, `slot`, `td`, `template`, `th`|[contentfirsttags.c](src/main/c/rule/tag/contentfirsttags.c)|
|`$formatting`|`a`, `abbr`, `b`, `bdi`, `bdo`, `cite`, `data`, `del`, `dfn`, `em`, `i`, `ins`, `kbd`, `mark`, `q`, `rp`, `rt`, `rtc`, `ruby`, `s`, `samp`, `small`, `span`, `strong`, `sub`, `sup`, `time`, `u`, `var`, `wbr`|[formattingtags.c](src/main/c/rule/tag/formattingtags.c)|
|`$layout`|`blockquote`, `body`, `colgroup`, `datalist`, `dialog`, `div`, `dl`, `fieldset`, `figure`, `footer`, `form`, `head`, `header`, `hgroup`, `html`, `main`, `map`, `menu`, `nav`, `ol`, `optgroup`, `picture`, `section`, `select`, `table`, `tbody`, `tfoot`, `thead`, `tr`, `ul`|[layouttags.c](src/main/c/rule/tag/layouttags.c)|
|`$specific`|All [SVG tags](src/main/c/rule/tag/svgtags.c), `area`, `base`, `br`, `code`, `col`, `embed`, `hr`, `img`, `input`, `param`, `pre`, `script`, `source`, `track`|[specifictags.c](src/main/c/rule/tag/specifictags.c)|
|`$heading`|`hgroup`, `h1`, `h2`, `h3`, `h4`, `h5`, `h6`|[headingtags.c](src/main/c/rule/tag/headingtags.c)|
|`$media`|`audio`, `video`|[mediatags.c](src/main/c/rule/tag/mediatags.c)|
|`$sectioning`|`article`, `aside`, `nav`, `section`|[sectioningtags.c](src/main/c/rule/tag/sectioningtags.c)|
|`$void`|`area`, `base`, `br`, `col`, `embed`, `hr`, `img`, `input`, `keygen`, `link`, `meta`, `param`, `source`, `track`, `wbr`|[voidtags.c](src/main/c/rule/tag/voidtags.c)|
|`$wss`|`pre`, `code`|[wsstags.c](src/main/c/rule/tag/wsstags.c)|

#### `--MXcollapseWhitespace $wss`

Reduce a sequence of whitespace characters in text nodes to a single space (U+0020), unless they are a child of the tags specified by this option.

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

#### `--MXdestroyWholeWhitespace $wss,$content,$formatting`

Remove any text nodes that only consist of whitespace characters, unless they are a child of the tags specified by this option.

Especially useful when using `display: inline-block` so that whitespace between elements (e.g. indentation) does not alter layout and styling.

<table><thead><tr><th>Before<th>After<tbody><tr><td>

```html
<div>↵
··<h1></h1>↵
··<ul></ul>↵
··A·quick·<strong>brown</strong>·<em>fox</em>.↵
</div>
```

<td>

```html
<div><h1></h1><ul></ul>↵
··A·quick·<strong>brown</strong><em>fox</em>.↵
</div>
```

</table>

#### `--MXtrimWhitespace $wss,$formatting`

Remove any whitespace from the start and end of a tag, if the first and/or last node is a text node, unless the tag is one of the tags specified by this option.

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

#### `--MXtrimClassAttribute`

Don't trim and collapse whitespace in `class` attribute values.

<table><thead><tr><th>Before<th>After<tbody><tr><td>

```html
<div class="
  hi
  lo
  a b c
    d e
  f   g
"></div>
```

<td>

```html
<div class="hi lo a b c d e f g"></div>
```

</table>

#### `--MXdecEnt`

Don't decode any valid entities into their UTF-8 values.

#### `--MXcondComments`

Don't minify the contents of conditional comments, including downlevel-revealed conditional comments.

#### `--MXattrQuotes`

Don't remove quotes around attribute values when possible.

#### `--MXcomments`

Don't remove any comments. Conditional comments are never removed regardless of this setting.

#### `--MXoptTags`

Don't remove optional starting or ending tags.

#### `--MXtagWS`

Don't remove spaces between attributes when possible.

### Non-options

#### Explicitly important

The following removal of attributes and tags as minification strategies are not available in hyperbuild, as they should not have been declared in the first place.

If they exist, it is assumed there is a special reason for being so.

- Remove empty attributes (including ones that would be empty after minification e.g. `class="   "`)
- Remove empty elements
- Remove redundant attributes
- Remove `type` attribute on `<script>` tags
- Remove `type` attribute on `<style>` and `<link>` tags
