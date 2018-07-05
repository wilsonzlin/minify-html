# Tag omission

|Tag name|Details|
|---|---|
|`li`| The end tag can be omitted if the list item is immediately followed by another `<li>` element, or if there is no more content in its parent element.|
|`p`| The start tag is required. The end tag may be omitted if the `<p>` element is immediately followed by an `<address>`, `<article>`, `<aside>`, `<blockquote>`, `<div>`, `<dl>`, `<fieldset>`, `<footer>`, `<form>`, `<h1>`, `<h2>`, `<h3>`, `<h4>`, `<h5>`, `<h6>`, `<header>`, `<hr>`, `<menu>`, `<nav>`, `<ol>`, `<pre>`, `<section>`, `<table>`, `<ul>` or another `<p>` element, or if there is no more content in the parent element and the parent element is not an `<a>` element.|
|`rp`| The end tag can be omitted if the element is immediately followed by an `<rt>` or another `<rp>` element, or if there is no more content in the parent element.|
|`rt`| The end tag may be omitted if the `<rt>` element is immediately followed by an `<rt>` or `<rp>` element, or if there is no more content in the parent element.|
|`rtc`| The closing tag can be omitted if it is immediately followed by a `<rb>`, `<rtc>` or `<rt>` element opening tag or by its parent closing tag.|
|`caption`| The end tag can be omitted if the element is not immediately followed by ASCII whitespace or a comment.|
|`colgroup`| The start tag may be omitted, if it has a `<col>` element as its first child and if it is not preceded by a `<colgroup>` whose end tag has been omitted. The end tag may be omitted, if it is not followed by a space or a comment.|
|`tbody`| The `<tbody>` element is not a required child element for a parent `<table>` element to graphically render. However, it must not be present, if its parent `<table>` element has a `<tr>` element as a child.|
|`td`| The start tag is mandatory. The end tag may be omitted, if it is immediately followed by a `<th>` or `<td>` element or if there are no more data in its parent element.|
|`tfoot`|The start tag is mandatory. The end tag may be omitted if there is no more content in the parent `<table>` element.|
|`th`|The start tag is mandatory. The end tag may be omitted, if it is immediately followed by a `<th>` or `<td>` element or if there are no more data in its parent element.|
|`thead`|The start tag is mandatory. The end tag may be omitted if the `<thead>` element is immediately followed by a `<tbody>` or `<tfoot>` element.|
|`tr`|Start tag is mandatory. End tag may be omitted if the `<tr>` element is immediately followed by a `<tr>` element, or if the row is the last element in its parent table group (`<thead>`, `<tbody>` or `<tfoot>`) element.|
|`optgroup`|The start tag is mandatory. The end tag is optional if this element is immediately followed by another `<optgroup>` element, or if the parent element has no more content.|
|`option`|The start tag is mandatory. The end tag is optional if this element is immediately followed by another `<option>` element or an `<optgroup>`, or if the parent element has no more content.|
