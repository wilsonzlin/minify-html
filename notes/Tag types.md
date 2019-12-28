# Tag types

|Type|Expected content|
|---|---|
|Formatting tags|Text nodes.|
|Content tags|Formatting tags, text nodes.|
|Layout tags|Layout tags, content tags.|
|Content-first tags|Content of content tags or layout tags (but not both).|

## Formatting tags

```html
<strong> moat </strong>
```

## Content tags

```html
<p>Some <strong>content</strong></p>
```

## Content-first tags

```html
<li>Anthony</li>
```

```html
<li>
  <div>
  </div>
</li>
```

## Layout tags

```html
<div>
  <div></div>
</div>
```
