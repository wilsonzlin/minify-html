# minhtml

CLI for [minify-html](https://github.com/wilsonzlin/minify-html).

## Usage

```
minhtml [FLAGS] [OPTIONS] [inputs]...
```

- **-o, --output \<output>**: Output destination; omit for stdout
- **\<inputs>...**: Files to minify; omit for stdin. If more than one is provided, they will be parallel minified in place, and --output must be omitted

### Flags

|Arg|Description|
|---|---|
|--do-not-minify-doctype|Do not minify DOCTYPEs. Minified DOCTYPEs may not be spec compliant|
|--ensure-spec-compliant-unquoted-attribute-values|Ensure all unquoted attribute values in the output do not contain any characters prohibited by the WHATWG specification|
|-h, --help|Prints help information|
|--keep-closing-tags|Do not omit closing tags when possible|
|--keep-comments|Keep all comments|
|--keep-html-and-head-opening-tags|Do not omit `<html>` and `<head>` opening tags when they don't have attributes|
|--keep-spaces-between-attributes|Keep spaces between attributes when possible to conform to HTML standards|
|--minify-css|Minify CSS in `<style>` tags and `style` attributes|
|--minify-css-level-1|Use optimisation level 1 for the CSS minifier|
|--minify-css-level-2|Use optimisation level 2 for the CSS minifier. May perform some dangerous optimisations|
|--minify-css-level-3|Use optimisation level 3 for the CSS minifier. May perform many dangerous optimisations|
|--minify-js|Minify JS in `<script>` tags that have a valid or no `type` attribute value|
|--remove-bangs|Remove all bangs|
|--remove-processing-instructions|Remove all processing_instructions|
|-V, --version|Prints version information|
