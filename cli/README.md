

## Parsing

Current limitations:

### Errors

Errors marked with a `⌫` can be suppressed using the [`--suppress`](#--suppress) option.

#### `MALFORMED_ENTITY` ⌫

It's an error if the sequence of characters following an ampersand (`&`) does not form a valid entity.

Entities must be of one of the following forms:

- `&name;`, where *name* is a reference to a valid HTML entity
- `&#nnnn;`, where *nnnn* is a Unicode code point in base 10
- `&#xhhhh;`, where *hhhh* is a Unicode code point in base 16

A malformed entity is an ampersand not followed by a sequence of characters that matches one of the above forms. This includes when the semicolon is missing.

Note that this is different from `INVALID_ENTITY`, which is when a well-formed entity references a non-existent entity name or Unicode code point.

While an ampersand by itself (i.e. followed by whitespace or as the last character) is a malformed entity, it is covered by `BARE_AMPERSAND`.

#### `BARE_AMPERSAND` ⌫

It's an error to have an ampersand followed by whitespace or as the last character.

This is intentionally a different error to `MALFORMED_ENTITY` due to the ubiquity of bare ampersands.

An ampersand by itself is not *necessarily* an invalid entity. However, HTML parsers and browsers may have different interpretations of bare ampersands, so it's a good idea to always use the encoded form (`&amp;`).

When this error is suppressed, bare ampersands are outputted untouched.

#### `INVALID_ENTITY` ⌫

It's an error if an invalid HTML entity is detected.

If suppressed, invalid entities are outputted untouched.

See [entityrefs.c](src/main/c/rule/entity/entityrefs.c) for the list of entity references considered valid by hyperbuild.

Valid entities that reference a Unicode code point must be between 0x0 and 0x10FFFF (inclusive).

#### `NONSTANDARD_TAG` ⌫

It's an error if an unknown (non-standard) tag is reached.
See [tags.c](src/main/c/rule/tag/tags.c) for the list of tags considered valid by hyperbuild.

#### `UCASE_TAG` ⌫

It's an error if an opening or closing tag's name has any uppercase characters.

#### `UCASE_ATTR` ⌫

It's an error if an attribute's name has any uppercase characters.

#### `UNQUOTED_ATTR` ⌫

It's an error if an attribute's value is not quoted with `"` (U+0022) or `'` (U+0027).
This means that `` ` `` is not a valid quote mark regardless of whether this error is suppressed or not. Backticks are valid attribute value quotes in Internet Explorer.

#### `ILLEGAL_CHILD`

It's an error if a tag is declared where it can't be a child of.
This is a very simple check, and does not cover the comprehensive HTML rules, which involve backtracking, tree traversal, and lots of conditionals.

This rule is enforced in four parts:
[whitelistparents.c](src/main/c/rule/relation/whitelistparents.c),
[blacklistparents.c](src/main/c/rule/relation/blacklistparents.c),
[whitelistchildren.c](src/main/c/rule/relation/whitelistchildren.c), and
[blacklistchildren.c](src/main/c/rule/relation/blacklistchildren.c).

#### `UNCLOSED_TAG`

It's an error if a non-void tag is not closed.
See [voidtags.c](src/main/c/rule/tag/voidtags.c) for the list of tags considered void by hyperbuild.

This includes tags that close automatically because of siblings (e.g. `<li><li>`), as it greatly simplifies the complexity of the minifier due to guarantees about the structure.

#### `SELF_CLOSING_TAG` ⌫

It's an error if a tag is self-closed. Valid in XML, not in HTML.

#### `NO_SPACE_BEFORE_ATTR`

It's an error if there is no whitespace before an attribute.

Most likely, the cause of this error is either invalid syntax or something like:

```html
<div class="a"name="1"></div>
```

(Note the lack of space between the end of the `class` attribute and the beginning of the `name` attribute.)

#### `UNEXPECTED_END` and `EXPECTED_NOT_FOUND`

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

Path to a file to process. If omitted, hyperbuild will read from `stdin`.

#### `--out`

Path to a file to write to; it will be created if it doesn't exist already. If omitted, the output will be streamed to `stdout`.

#### `--keep`

Don't automatically delete the output file if an error occurred. If the output is `stdout`, or the output is a file but `--buffer` is provided, this option does nothing.

#### `--buffer`

Buffer all output until the process is complete and successful. This won't truncate or write anything to the output until the build process is done, but will use a non-constant amount of memory.
This applies even when the output is `stdout`.

#### `--suppress`

Suppress errors specified by this option. hyperbuild will quitely ignore and continue processing when otherwise one of the provided errors would occur.

Suppressible errors are marked with a `⌫` in the [Errors](#errors) section. Omit the `` prefix. Separate the error names with commas.



### Options

Note that only existing whitespace will be up for removal via minification. Entities that represent whitespace will not be decoded and then removed.

For options that have a list of tags as their value, the tags should be separated by a comma.

An `*` (asterisk, U+002A) can be used to represent the complete set of possible tags. Providing no value represents the empty set.
Both values essentially fully enables or disables the option.

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

As an example, for `--MXcollapseWhitespace`, here are some possible values:

|Arguments|Description|
|---|---|
|`--MXcollapseWhitespace $wss`|Collapse whitespace in all tags except `$wss` ones|
|`--MXcollapseWhitespace $content,$wss`|Collapse whitespace in all tags except `$content` and `$wss` ones|
|`--MXcollapseWhitespace $content,$wss,dd`|Collapse whitespace in all tags except `$content` and `$wss` ones, as well as the `dd` tag|
|`--MXcollapseWhitespace sup,dd`|Collapse whitespace in all tags except `sup` and `dd`|
|`--MXcollapseWhitespace`|Collapse whitespace in all tags|
|`--MXcollapseWhitespace *`|Don't collapse whitespace in any tag|

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

The following removal of attributes and tags as minification strategies are not available in hyperbuild, as it is assumed there is a special reason for their declaration:

- empty attributes (including ones that would be empty after minification e.g. `class="   "`)
- empty elements
- redundant attributes
- `type` attribute on `<script>` tags
- `type` attribute on `<style>` and `<link>` tags
