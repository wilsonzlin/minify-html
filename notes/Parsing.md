# Parsing

minify-html does not have any error states and will always return a string. This means that all possible ambiguous or malformed states need to be handled. This document describes these.

minify-html tries to match what modern browsers do (which is not necessarily what the spec says). However, there may be occasional differences for malformed syntax, as browsers have extremely more complex parsers and rules.

To see some complex inputs, check out the [various fuzzing inputs](../fuzz/in).

## EOF

If the input ends while in the middle of a tag or attribute value, that tag/attribute is closed, as well as all ancestor tags.

## Tags

|Rule|Example source|Example interpretation|
|---|---|---|
|`script`, `style`, and `textarea` tags do not close until the case-insensitive sequence `</` followed by the tag name.|`<teXTaRea></textare></TEXTArea>`|`<textarea></textare></textarea>`|
|Attribute-like syntax in closing tags are parsed like attributes but ignored.|`<div></div x=">">5`|`<div></div>`|
|If the character following `</` is not a valid tag name character, all code until the next `>` is dropped. It is not considered a closing tag, not even as an invalid one.|`<div></   div x=">">5`|`<div>">5`
|If a closing tag represents a void element, the closing tag is dropped.|`<div><br>ax</br><img></img>i</div>`|`<div><br>ax<img>i</div>`|
|If a closing tag does not match the opening tag, and the closing tag cannot be omitted as per the spec, the closing tag is reinterpreted as an opening tag. Most browsers have much more complex rules, depending on tag name and ancestors.|`<div><span></div></span>5`|`<div><span><div><span>5`|
|If an opening tag ends with `/>` instead of `>`, and it's an HTML tag, the `/` is ignored. If it's an SVG tag, it's self-closing.|`<div/>5<div></div>`|`<div>5<div></div>`|
|A slash as the last character of an unquoted attribute value immediately preceding a `>` is not interpreted as part of the self-closing syntax `/>`, even for self-closable SVG elements.|`<circle r=1/>`|`<circle r="1/">`|
|Any opening `html`, `head`, or `body` tags after the first are ignored.|`<html><head><meta><body><div><head><span><body>`|`<html><head><meta><body><div><span>`|
|Any closing `html`, `head`, or `body` tags are ignored.|`<html><head><meta><body><div></body><span></body><input></html><a>`|`<html><head><meta><body><div><span><input><a>`|
|If a `<` in content is not followed by an alphanumeric, `:`, or `=` character, it is interpreted as a literal `<`, as per the [spec](https://html.spec.whatwg.org/multipage/syntax.html#syntax-tag-name)|`<div>< /div>< span>`|`<div>< /div>< span>`|

## Attributes

|Rule|Example source|Example interpretation|
|---|---|---|
|Whitespace can exist between an `=` and the attribute name and value.|`a   =  =b=`|`a="=b="`|
|An unquoted attribute value continues until the next `>`, `/`, or whitespace character.|`a = b"cdef/>`|`a='b"cdef' />`|
|Whitespace and slashes separate attributes, but not around `=`.|`a  = b /c/d==/e=/f`|`a="b" c="" d="=" e="/f"`|
|An attribute name is every character until the next `=`, `/`, `>`, or whitespace character.|`"a":  {}#$'=/>`|`"a":="" {}#$'="" />`|
