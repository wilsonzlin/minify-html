# minify-html changelog

## 0.6.9

- Intrepret `type=module` on `<script>` tags as a JavaScript MIME eligible for its contents to be minified as JavaScript (previously it would not be and so its contents would be considered data and never minified as JavaScript).
