# Script data

## Summary

For legacy reasons, HTML comments can appear within a script tag, and if there is a `<script` in it, the first following `</script>` within the comment does **not** close the main script tag.

minify-html does **not** do this special handling, as it adds complexity and slows down performance dramatically, for a legacy feature that is not recommended to be (and almost never) used.

See https://www.w3.org/TR/html52/syntax.html#script-data-state for more details.

Commit [20c59769](https://github.com/wilsonzlin/minify-html/commit/20c59769fea6bfb8a9d5ecea47d979dc9b1dcda5) removed support.

## States and transitions

|State|`<script`|`</script`|`<!--`|`-->`|
|---|---|---|---|---|
|Normal|-|End|Escaped|-|
|Escaped|DoubleEscaped|End|-|Normal|
|DoubleEscaped|-|Escaped|-|Normal|

## Examples

```html
<script type="text/html">
  <script>
    exec1 = true;
  </script>
  <script>
    exec2 = true;
  </script>
</script>
```
These are true about the above snippet:
- `document.querySelectorAll('script').length === 2`.
- `!window.exec1 && window.exec2`.
- `document.querySelector('script[type="text/html"]')` has exactly one child node and it's a text node.

If there is one or more `<script>` inside an HTML comment before any `</script>`, the first `</script>` will not end the main script.

Ending tag inside comment works because there are no nested script tags.

```html
<script type="text/plain"><!--
</script>
```

There is a nested tag but it doesn't need to be closed because it's treated as text.

```html
<script type="text/plain">
  <script>
</script>
```

There is a nested tag but it doesn't need to be closed because it's not inside an HTML comment.

```html
<script type="text/plain">
  <script><!--
</script>
```

There is a nested tag but it doesn't need to be closed because main closing tag is not inside an HTML comment.

```html
<script type="text/plain"><!--
  <script>-->
</script>
```

The main closing tag is in a HTML comment but still works because there is no `<script>` before it in its comment (there is one in the previous comment).

```html
<script type="text/plain">
    <!--<script>--><!--
</script>
```

There is a nested tag and it's in an HTML comment but it doesn't need to be closed because it's not `script`.

```html
<script type="text/plain"><!--
  <div>
</script>
```

First closing tag is inside a comment with one or more previous opening tags so it doesn't close main script tag.

```html
<script type="text/plain"><!--
  <script>alert();</script>
</script>
```

There is a nested tag and it needs to be closed because it's in a comment `script`. The amount of `<script>` doesn't matter.

```html
<script type="text/plain"><!--
  <script><script><script><script><script>alert();</script>
</script>
```

First and second closing tags close their respective previous 1+ opening tags.

```html
<script type="text/plain"><!--
  <script><script><script>alert();</script>
  <script></script>
</script>
```

Main closing tag works because it is in a separate comment.

```html
<script type="text/plain">
  <!--<script><script><script>--><!--
</script>
```

Main closing tag works because it is not in a comment.

```html
<script type="text/plain">
  <!--<script>-->
</script>
```
