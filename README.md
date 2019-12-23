# hyperbuild

A fast one-pass in-place HTML minifier written in Rust with advanced whitespace handling.

Currently in beta, working on documentation and tests. Issues and pull requests welcome!

## Features

- Minification is done in one pass with no backtracking or DOM/AST building.
- No dynamic memory is allocated during processing, which increases safety and performance.
- Advanced whitespace handling options allow maximum minification while retaining valid spaces.

## Usage

TODO

## Minification

Guide below is currently WIP.

### Whitespace

hyperbuild has advanced whitespace minification that can allow strategies such as:

- leave whitespace untouched in `pre` and `code`, which are whitespace sensitive
- trim and collapse whitespace in content tags, as whitespace is collapsed anyway when rendered
- remove whitespace in layout tags, which allows the use of inline layouts while keeping formatted code

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
  
## Development

Currently, hyperbuild has a few limitations:

- Only UTF-8 is supported.
- Not aware of exotic Unicode whitespace characters.
- Follows HTML5 only.
- Only works on Linux.

Patches to change any of these welcome!
