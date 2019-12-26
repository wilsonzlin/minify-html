#### Beginning and end

```html
<p>↵
··The·quick·brown·fox↵
</p>
```

#### Between text and tags

```html
<p>The·quick·brown·fox·<strong>jumps</strong>·over·the·lazy·dog.</p>
```

#### Contiguous

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

#### Whole text

```html
<p>↵
···↵
</p>
```

### Tag classification

|Type|Content|
|---|---|
|Formatting tags|Text nodes|
|Content tags|Formatting tags, text nodes|
|Layout tags|Layout tags, content tags|
|Content-first tags|Content of content tags or layout tags (but not both)|

#### Specific tags

Tags not in one of the categories below are **specific tags**.

#### Formatting tags

```html
<strong> moat </strong>
```

#### Content tags

```html
<p>Some <strong>content</strong></p>
```

#### Content-first tags

```html
<li>Anthony</li>
```

```html
<li>
  <div>
  </div>
</li>
```

#### Layout tags

```html
<div>
  <div></div>
</div>
```

### Options

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
