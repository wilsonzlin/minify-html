# hyperbuild

A fast HTML parser, preprocessor, and minifier, written in C.
Designed to be used in C projects, but also runnable on Node.js thanks to Emscripten.
Minifier heavily influenced by [kangax's html-minifier](https://github.com/kangax/html-minifier).

## Features

### Streaming minification

hyperbuild minifies as it parses, directly streaming processed HTML to the output without having to build a DOM/AST or iterate/traverse around in multiple passes, allowing for super-fast compilation times and near-constant memory usage.

### Smart parsing

hyperbuild is aware of strings and comments in JS and CSS sections, and deals with them correctly.

### Super low level

hyperbuild is written in C, and exposed to Node.js using Emscripten.

## Parsing

Current limitations:

- UTF-8 in, UTF-8 out, no BOM at any time.
- Not aware of exotic Unicode whitespace characters.
- Tested and designed for Linux only.
- Follows HTML5 only.

### Errors

Errors marked with a `⌫` can be suppressed using the [`--errorEx`](#--errorex) option.

#### `EBADENT` ⌫

It's an error if an invalid HTML entity is detected.
If suppressed, invalid entities are simply interpreted literally.
See [entityrefs.c](src/main/c/rule/entity/entityrefs.c) for the list of entity references considered valid by hyperbuild.
Valid entities that reference a Unicode code point must be between 0x0 and 0x10FFFF (inclusive).

#### `EBADTAG` ⌫

It's an error if an unknown (non-standard) tag is reached.
See [tags.c](src/main/c/rule/tag/tags.c) for the list of tags considered valid by hyperbuild.

#### `EUCASETAG` ⌫

It's an error if an opening or closing tag's name has any uppercase characters.

#### `EUCASEATTR` ⌫

It's an error if an attribute's name has any uppercase characters.

#### `EUQOTATTR` ⌫

It's an error if an attribute's value is not quoted with `"` (U+0022).
This means that `` ` `` and `'` are not valid quote marks.

#### `EBADCHILD`

It's an error if a tag is declared where it can't be a child of.
This is a very simple check, and does not cover the comprehensive HTML rules, as they involve backtracking, tree traversal, and lots of conditionals.

This rule is enforced in four parts:
[whitelistparents.c](src/main/c/rule/relation/whitelistparents.c),
[blacklistparents.c](src/main/c/rule/relation/blacklistparents.c),
[whitelistchildren.c](src/main/c/rule/relation/whitelistchildren.c), and
[blacklistchildren.c](src/main/c/rule/relation/blacklistchildren.c).

#### `EUNCTAG`

It's an error if a non-void tag is not closed.
See [voidtags.c](src/main/c/rule/tag/voidtags.c) for the list of tags considered void by hyperbuild.

This includes tags that close automatically because of siblings (e.g. `<li><li>`), as it greatly simplifies the complexity of the minifier due to guarantees about the structure.

#### Additional errors

There are additional implicit errors that are considered as generic syntax errors due to the way the parser works:

- Closing void tags; see [voidtags.c](src/main/c/rule/tag/voidtags.c) for the list of tags considered void by hyperbuild.
- Self closing tags.
- Placing whitespace between `=` and attribute names/values.
- Placing whitespace before the tag name in an opening tag.
- Placing whitespace around the tag name in a closing tag.

### Options

#### `--in`

Path to a file to process. If omitted, hyperbuild will read from `stdin`, and imports will be relative to the working directory.

#### `--out`

Path to a file to write to; it will be created if it doesn't exist already. If omitted, the output will be streamed to `stdout`.

#### `--keep`

Don't automatically delete the output file if an error occurred. This option does nothing if the output is `stdout`, and cannot be used with `--buffer`.

#### `--buffer`

Buffer all output until the process is complete and successful. This can prevent many writes to storage (and won't cause any writes on error), but will use a non-constant amount of memory.
This applies even when the output is `stdout`, and cannot be used with `--keep`.

#### `--errorEx`

Suppress errors specified by this option. hyperbuild will quitely ignore and continue processing when otherwise one of the provided errors would occur.

Separate the error names by a comma. Suppressible errors are marked with a `⌫` in the [Errors](#errors) section.

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
- To use commas or right parentheses in argument values, use the HTML entity (`&#44;` and `&#41;`)
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

##### Between text and tags

##### Contiguous

##### Whole text

#### Content

##### Specific tags

```html
<title></title>
<meta>
<link>
<table></table>
<ul></ul>
<ol></ol>
<dl></dl>
<hr>
<br>
```

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

##### Sectioning tags

```html
<p>Some <strong>content</strong></p>
```

##### Overview

|Type|Content|
|---|---|
|Formatting tags|Text nodes|
|Content tags|Formatting tags, text nodes|
|Sectioning tags|Sectioning tags, formatting tags|

|Tag|Formatting|Content|Sectioning|
|---|---|---|---|
|`li`, `dd`|-|1|2|
|`p`, `h1-6`|-|Y|-|

### Options

For options that have a list of tags as their values, the tags should be separated by a comma.
For brevity, hyperbuild has built-in sets of tags that can be used in place of declaring all their members; they begin with a `$` sign:

|Name|Tags|Description|
|---|---|---|
|`$wss`|`pre`, `code`|Whitespace sensitive.|
|`$content`|`p`, `h1`, `h2`, `h3`, `h4`, `h5`, `h6`|Content tags.|
|`$formatting`|`a`, `abbr`, `b`, `bdi`, `bdo`, `cite`, `code`, `data`, `dfn`, `em`, `i`, `kbd`, `mark`, `q`, `rt`, `rtc`, `ruby`, `s`, `samp`, `small`, `span`, `strong`, `sub`, `sup`, `time`, `u`, `var`, `wbr`|Inline text semantics (see https://developer.mozilla.org/en-US/docs/Web/HTML/Element#Inline_text_semantics).|

#### `--collapseWhitespaceEx $wss`

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

#### `--destroyWholeWhitespaceEx $wss,$content,$formatting`

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

#### `--trimWhitespaceEx $wss,$formatting`

Remove any whitespace from the start and end of a tag, if the first and/or last node is a text node, unless the tag is one of the tags specified by this option.

Useful when combined with whitespace collapsing.

Other whitespace between text nodes and tags are not removed, as it is not recommended to mix non-inline tags with raw text; wrap text in an appropriate tag.
Basically, a tag should only either contain text and [inline text semantics](https://developer.mozilla.org/en-US/docs/Web/HTML/Element#Inline_text_semantics), or tags.

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

#### `--trimClassAttribute`

Trim and collapse whitespace in `class` attribute values.

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

#### `--decodeEntities`

Decode any valid entities into their UTF-8 values.

#### `--processConditionalComments`

Process the contents of conditional comments, including downlevel-revealed conditional comments.

#### `--removeAttributeQuotes`

Remove quotes around attribute values when possible.

#### `--removeComments`

Remove any comments, except conditional comments.

#### `--removeOptionalTags`

Remove optional starting or ending tags.

#### `--removeTagWhitespace`

Remove spaces between attributes when possible.

### Non-options

#### Collapse boolean attributes

Not provided, as they should not have been declared in the first place.
(If they exist, it is assumed there is a special reason for being so.)

#### Remove empty attributes

#### Remove empty elements

#### Remove redundant attributes

#### Remove `type` attribute on `<script>` tags

#### Remove `type` attribute on `<style>` and `<link>` tags
