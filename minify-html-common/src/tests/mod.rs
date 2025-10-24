use std::collections::HashMap;

pub fn create_common_test_data() -> HashMap<&'static [u8], &'static [u8]> {
  let mut t = HashMap::<&'static [u8], &'static [u8]>::new();

  // collapse whitespace
  t.insert(b"<a>   \n&#32;   </a>", b"<a> </a>");
  // Tag names should be case insensitive.
  t.insert(b"<A>   \n&#32;   </a>", b"<a> </a>");
  t.insert(b"<a>   \n&#32;   </A>", b"<a> </a>");

  // collapse and trim whitespace
  t.insert(b"<label>   \n&#32;   </label>", b"<label></label>");
  t.insert(b"<label>   \n&#32;a   </label>", b"<label>a</label>");
  t.insert(b"<label>   \n&#32;a   b   </label>", b"<label>a b</label>");
  // Tag names should be case insensitive.
  t.insert(b"<lAbEL>   \n&#32;a   b   </LABel>", b"<label>a b</label>");

  // collapse destroy whole and trim whitespace
  t.insert(b"<ul>   \n&#32;   </ul>", b"<ul></ul>");
  t.insert(b"<ul>   \n&#32;a   </ul>", b"<ul>a</ul>");
  t.insert(b"<ul>   \n&#32;a   b   </ul>", b"<ul>a b</ul>");
  t.insert(
    b"<ul>   \n&#32;a<pre></pre>   <pre></pre>b   </ul>",
    b"<ul>a<pre></pre><pre></pre>b</ul>",
  );
  t.insert(
    b"<svg> <path> </path> <path> </path> </svg>",
    b"<svg><path></path><path></path></svg>",
  );
  // Tag names should be case insensitive.
  t.insert(b"<uL>   \n&#32;a   b   </UL>", b"<ul>a b</ul>");

  // no whitespace minification
  t.insert(b"<pre>   \n&#32; \t   </pre>", b"<pre>   \n  \t   </pre>");
  t.insert(
    b"<textarea>   \n&#32; \t   </textarea>",
    b"<textarea>   \n  \t   </textarea>",
  );
  // Tag names should be case insensitive.
  t.insert(b"<pRe>   \n&#32; \t   </PRE>", b"<pre>   \n  \t   </pre>");
  t.insert(
    b"<pre>  <span>  1    2   </span>  </pre>",
    b"<pre>  <span>  1    2   </span>  </pre>",
  );
  t.insert(
    b"<pre>  <span>  1 <pre>\n</pre>    2   </span>  </pre>",
    b"<pre>  <span>  1 <pre>\n</pre>    2   </span>  </pre>",
  );
  t.insert(
    b"<div>  <pre>  <span>  1 <pre>\n</pre>    2   </span>  </pre>  </div>",
    b"<div><pre>  <span>  1 <pre>\n</pre>    2   </span>  </pre></div>",
  );
  t.insert(
    br#"<pre><code>fn main() {
  println!("Hello, world!");
  <span>loop {
    println!("Hello, world!");
  }</span>
</code></pre>"#,
    br#"<pre><code>fn main() {
  println!("Hello, world!");
  <span>loop {
    println!("Hello, world!");
  }</span>
</code></pre>"#,
  );

  // parsing omitted closing tag
  t.insert(b"<html>", b"<html>");
  t.insert(b" <html>\n", b"<html>");
  t.insert(
    b"<html><div> <p>Foo</div></html>",
    b"<html><div><p>Foo</div>",
  );

  // self closing svg tag whitespace removal
  t.insert(b"<svg><path d=a /></svg>", b"<svg><path d=a /></svg>");
  t.insert(b"<svg><path d=a/ /></svg>", b"<svg><path d=a/ /></svg>");
  t.insert(b"<svg><path d=\"a/\" /></svg>", b"<svg><path d=a/ /></svg>");
  t.insert(b"<svg><path d=\"a/\"/></svg>", b"<svg><path d=a/ /></svg>");
  t.insert(b"<svg><path d='a/' /></svg>", b"<svg><path d=a/ /></svg>");
  t.insert(b"<svg><path d='a/'/></svg>", b"<svg><path d=a/ /></svg>");

  // parsing with omitted tags
  t.insert(b"<ul><li>1<li>2<li>3</ul>", b"<ul><li>1<li>2<li>3</ul>");
  t.insert(b"<rt>", b"<rt>");
  t.insert(b"<rt><rp>1</rp><div></div>", b"<rt><rp>1</rp><div></div>");
  t.insert(b"<div><rt></div>", b"<div><rt></div>");
  t.insert(b"<html><head><body>", b"<html><head><body>");
  t.insert(b"<html><head><body>", b"<html><head><body>");
  // Tag names should be case insensitive.
  t.insert(b"<rt>", b"<rt>");

  // removal of optional tags
  t.insert(
    b"<ul><li>1</li><li>2</li><li>3</li></ul>",
    b"<ul><li>1<li>2<li>3</ul>",
  );
  t.insert(b"<rt></rt>", b"<rt>");
  t.insert(
    b"<rt></rt><rp>1</rp><div></div>",
    b"<rt><rp>1</rp><div></div>",
  );
  t.insert(b"<div><rt></rt></div>", b"<div><rt></div>");
  t.insert(
    br#"
        <html>
            <head>
            </head>

            <body>
            </body>
        </html>
    "#,
    b"<html><head><body>",
  );
  // Tag names should be case insensitive.
  t.insert(b"<RT></rt>", b"<rt>");

  // removal of optional closing p tag
  t.insert(b"<p></p><address></address>", b"<p><address></address>");
  t.insert(b"<p></p>", b"<p>");
  t.insert(b"<map><p></p></map>", b"<map><p></p></map>");
  t.insert(
    b"<map><p></p><address></address></map>",
    b"<map><p><address></address></map>",
  );

  // attr double quoted value minification
  t.insert(b"<a b=\" hello \"></a>", b"<a b=\" hello \"></a>");
  t.insert(b"<a b=' hello '></a>", b"<a b=\" hello \"></a>");
  t.insert(br#"<a b="/>aaaa"></a>"#, br#"<a b="/>aaaa"></a>"#);
  t.insert(br#"<a b="</a>a"></a>"#, br#"<a b="</a>a"></a>"#);
  t.insert(b"<a b=&#x20;hello&#x20;></a>", b"<a b=\" hello \"></a>");
  t.insert(b"<a b=&#x20hello&#x20></a>", b"<a b=\" hello \"></a>");

  // attr single quoted value minification
  t.insert(b"<a b=\"&quot;hello\"></a>", b"<a b='\"hello'></a>");
  t.insert(b"<a b='\"hello'></a>", b"<a b='\"hello'></a>");
  t.insert(b"<a b='/>a'></a>", b"<a b=\"/>a\"></a>");
  t.insert(
    b"<a b=&#x20;he&quot;llo&#x20;></a>",
    b"<a b=' he\"llo '></a>",
  );

  // attr unquoted value minification
  t.insert(b"<a b=\"hello\"></a>", b"<a b=hello></a>");
  t.insert(b"<a b='hello'></a>", b"<a b=hello></a>");
  t.insert(b"<a b=/&gt></a>", br#"<a b="/>"></a>"#);
  t.insert(b"<a b=/&gt&lt;a></a>", br#"<a b="/><a"></a>"#);
  t.insert(b"<a b=hello></a>", b"<a b=hello></a>");

  // class attr value minification
  t.insert(b"<a class=&#x20;c></a>", b"<a class=c></a>");
  t.insert(
    b"<a class=&#x20;c&#x20&#x20;d&#x20></a>",
    b"<a class=\"c d\"></a>",
  );
  t.insert(b"<a class=&#x20&#x20&#x20;&#x20></a>", b"<a></a>");
  t.insert(b"<a class=\"  c\n \n  \"></a>", b"<a class=c></a>");
  t.insert(b"<a class=\"  c\n \nd  \"></a>", b"<a class=\"c d\"></a>");
  t.insert(b"<a class=\"  \n \n  \"></a>", b"<a></a>");
  t.insert(b"<a class='  c\n \n  '></a>", b"<a class=c></a>");
  t.insert(b"<a class='  c\n \nd  '></a>", b"<a class=\"c d\"></a>");
  t.insert(b"<a class='  \n \n  '></a>", b"<a></a>");
  // Attribute names should be case insensitive.
  t.insert(b"<a CLasS='  \n \n  '></a>", b"<a></a>");

  // d attr value minification
  t.insert(b"<svg><path d=&#x20;c /></svg>", b"<svg><path d=c /></svg>");
  t.insert(
    b"<svg><path d=&#x20;c&#x20&#x20;d&#x20 /></svg>",
    b"<svg><path d=\"c d\"/></svg>",
  );
  t.insert(
    b"<svg><path d=&#x20;&#x20&#x20&#x20 /></svg>",
    b"<svg><path/></svg>",
  );
  t.insert(
    b"<svg><path d=\"  c\n \n  \" /></svg>",
    b"<svg><path d=c /></svg>",
  );
  t.insert(
    b"<svg><path d=\"  c\n \nd  \" /></svg>",
    b"<svg><path d=\"c d\"/></svg>",
  );
  t.insert(
    b"<svg><path d=\"  \n \n  \" /></svg>",
    b"<svg><path/></svg>",
  );
  t.insert(
    b"<svg><path d='  c\n \n  ' /></svg>",
    b"<svg><path d=c /></svg>",
  );
  t.insert(
    b"<svg><path d='  c\n \nd  ' /></svg>",
    b"<svg><path d=\"c d\"/></svg>",
  );
  t.insert(b"<svg><path d='  \n \n  ' /></svg>", b"<svg><path/></svg>");
  // Attribute names should be case insensitive.
  t.insert(b"<svg><path D='  \n \n  ' /></svg>", b"<svg><path/></svg>");

  // boolean attr value removal
  t.insert(b"<div hidden=\"true\"></div>", b"<div hidden></div>");
  t.insert(b"<div hidden=\"false\"></div>", b"<div hidden></div>");
  t.insert(b"<div hidden=\"1\"></div>", b"<div hidden></div>");
  t.insert(b"<div hidden=\"0\"></div>", b"<div hidden></div>");
  t.insert(b"<div hidden=\"abc\"></div>", b"<div hidden></div>");
  t.insert(b"<div hidden=\"\"></div>", b"<div hidden></div>");
  t.insert(b"<div hidden></div>", b"<div hidden></div>");
  // Attribute names should be case insensitive.
  t.insert(b"<div HIDden=\"true\"></div>", b"<div hidden></div>");

  // empty attr removal
  t.insert(b"<div lang=\"  \"></div>", b"<div lang=\"  \"></div>");
  t.insert(b"<div lang=\"\"></div>", b"<div></div>");
  t.insert(b"<div lang=''></div>", b"<div></div>");
  t.insert(b"<div lang=></div>", b"<div></div>");
  t.insert(b"<div lang></div>", b"<div></div>");

  // default attr value removal
  t.insert(b"<a target=\"_self\"></a>", b"<a></a>");
  t.insert(b"<a target='_self'></a>", b"<a></a>");
  t.insert(b"<a target=_self></a>", b"<a></a>");
  // Attribute names should be case insensitive.
  t.insert(b"<a taRGET='_self'></a>", b"<a></a>");

  // script type attr value removal
  t.insert(
    b"<script type=\"application/ecmascript\"></script>",
    b"<script></script>",
  );
  t.insert(
    b"<script type=\"application/javascript\"></script>",
    b"<script></script>",
  );
  t.insert(
    b"<script type=\"text/jscript\"></script>",
    b"<script></script>",
  );
  t.insert(
    b"<script type=\"text/plain\"></script>",
    b"<script type=text/plain></script>",
  );
  // Tag and attribute names should be case insensitive.
  t.insert(
    b"<SCRipt TYPE=\"application/ecmascript\"></SCrIPT>",
    b"<script></script>",
  );

  // empty attr value removal
  t.insert(b"<div a=\"  \"></div>", b"<div a=\"  \"></div>");
  t.insert(b"<div a=\"\"></div>", b"<div a></div>");
  t.insert(b"<div a=''></div>", b"<div a></div>");
  t.insert(b"<div a=></div>", b"<div a></div>");
  t.insert(b"<div a></div>", b"<div a></div>");

  // hexadecimal entity decoding
  t.insert(b"&#x2E", b".");
  t.insert(b"&#x2F", b"/");
  t.insert(b"&#x2f", b"/");
  t.insert(b"&#x00", b"\0");
  t.insert(b"&#x30", b"0");
  t.insert(b"&#x0030", b"0");
  t.insert(b"&#x000000000000000000000000000000000000000000030", b"0");
  t.insert(b"&#x30;", b"0");
  t.insert(b"&#x0030;", b"0");
  t.insert(b"&#x000000000000000000000000000000000000000000030;", b"0");
  t.insert(b"&#x1151;", b"\xe1\x85\x91");
  t.insert(b"&#x11FFFF;", b"\xef\xbf\xbd");
  t.insert(
    b"&#xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF;",
    b"\xef\xbf\xbd",
  );

  // decimal entity decoding
  t.insert(b"&#48", b"0");
  t.insert(b"&#0048", b"0");
  t.insert(b"&#000000000000000000000000000000000000000000048", b"0");
  t.insert(b"&#48;", b"0");
  t.insert(b"&#0048;", b"0");
  t.insert(b"&#000000000000000000000000000000000000000000048;", b"0");
  t.insert(b"&#4433;", b"\xe1\x85\x91");
  t.insert(b"&#1114112;", b"\xef\xbf\xbd");
  t.insert(
    b"&#999999999999999999999999999999999999999999999;",
    b"\xef\xbf\xbd",
  );

  // named entity decoding
  t.insert(b"&gt", b">");
  t.insert(b"&gt;", b">");
  t.insert(b"&amp", b"&");
  t.insert(b"&amp;", b"&");
  t.insert(b"&xxxyyyzzz", b"&xxxyyyzzz");
  t.insert(b"&ampere", b"&ere");
  t.insert(b"They & Co.", b"They & Co.");
  t.insert(b"if (this && that)", b"if (this && that)");
  // These entities decode to longer UTF-8 sequences, so we keep them encoded.
  t.insert(b"&nLt;", b"&nLt;");
  t.insert(b"&nLt;abc", b"&nLt;abc");
  t.insert(b"&nGt;", b"&nGt;");

  // Named entities not ending with ';' in attr values are not decoded if immediately
  // followed by an alphanumeric or `=` character. (See parser for more details.)
  t.insert(
    br#"<a href="exam ple?&gta=5"></a>"#,
    br#"<a href="exam ple?&gta=5"></a>"#,
  );
  t.insert(
    br#"<a href="exam ple?&gt=5"></a>"#,
    br#"<a href="exam ple?&gt=5"></a>"#,
  );
  t.insert(
    br#"<a href="exam ple?&gt~5"></a>"#,
    br#"<a href="exam ple?>~5"></a>"#,
  );

  // unintentional entity prevention
  t.insert(b"&ampamp", b"&ampamp");
  t.insert(b"&ampamp;", b"&ampamp;");
  t.insert(b"&amp;amp", b"&ampamp");
  t.insert(b"&amp;amp;", b"&ampamp;");
  t.insert(b"&&#97&#109;&#112;;", b"&ampamp;");
  t.insert(b"&&#97&#109;p;", b"&ampamp;");
  t.insert(b"&am&#112", b"&ampamp");
  t.insert(b"&am&#112;", b"&ampamp");
  t.insert(b"&am&#112&#59", b"&ampamp;");
  t.insert(b"&am&#112;;", b"&ampamp;");
  t.insert(b"&am&#112;&#59", b"&ampamp;");
  t.insert(b"&am&#112;&#59;", b"&ampamp;");

  t.insert(b"&l&#116", b"&amplt");
  t.insert(b"&&#108t", b"&amplt");
  t.insert(b"&&#108t;", b"&amplt;");
  t.insert(b"&&#108t&#59", b"&amplt;");
  t.insert(b"&amplt", b"&amplt");
  t.insert(b"&amplt;", b"&amplt;");

  t.insert(b"&am&am&#112", b"&am&ampamp");
  t.insert(b"&am&am&#112&#59", b"&am&ampamp;");

  t.insert(b"&amp&nLt;", b"&&nLt;");
  t.insert(b"&am&nLt;", b"&am&nLt;");
  t.insert(b"&am&nLt;a", b"&am&nLt;a");
  t.insert(b"&am&nLt", b"&am&nLt");

  // left chevron in content
  t.insert(b"<pre><</pre>", b"<pre><</pre>");
  t.insert(b"<pre>< </pre>", b"<pre>< </pre>");
  t.insert(b"<pre> < </pre>", b"<pre> < </pre>");

  t.insert(b"<pre> &lta </pre>", b"<pre> &LTa </pre>");
  t.insert(b"<pre> &lt;a </pre>", b"<pre> &LTa </pre>");
  t.insert(b"<pre> &LTa </pre>", b"<pre> &LTa </pre>");
  t.insert(b"<pre> &LT;a </pre>", b"<pre> &LTa </pre>");

  t.insert(b"<pre> &lt? </pre>", b"<pre> &LT? </pre>");
  t.insert(b"<pre> &lt;? </pre>", b"<pre> &LT? </pre>");
  t.insert(b"<pre> &LT? </pre>", b"<pre> &LT? </pre>");
  t.insert(b"<pre> &LT;? </pre>", b"<pre> &LT? </pre>");

  t.insert(b"<pre> &lt;/ </pre>", b"<pre> &LT/ </pre>");
  t.insert(b"<pre> &lt;! </pre>", b"<pre> &LT! </pre>");

  t.insert(b"&LT", b"<");
  t.insert(b"&LT;", b"<");
  t.insert(b"&LT;;", b"<;");
  t.insert(b"&LT;&#59", b"<;");
  t.insert(b"&LT;&#59;", b"<;");
  t.insert(b"&lt", b"<");
  t.insert(b"&lt;", b"<");
  t.insert(b"&lt;;", b"<;");
  t.insert(b"&lt;&#59", b"<;");
  t.insert(b"&lt;&#59;", b"<;");

  t.insert(b"&LTa", b"&LTa");
  t.insert(b"&LT;a", b"&LTa");
  t.insert(b"&LT;a;", b"&LTa;");
  t.insert(b"&LT;a&#59", b"&LTa;");
  t.insert(b"&LT;a&#59;", b"&LTa;");
  t.insert(b"&LT;a;&#59;", b"&LTa;;");

  t.insert(b"&lt;&#33", b"&LT!");
  t.insert(b"&lt;&#38", b"<&");
  t.insert(b"&lt;&#47", b"&LT/");
  t.insert(b"&lt;&#63", b"&LT?");
  t.insert(b"&lt;&#64", b"<@");

  // comments removal
  t.insert(
    b"<pre>a <!-- akd--sj\n <!-- \t\0f--ajk--df->lafj -->  b</pre>",
    b"<pre>a   b</pre>",
  );
  t.insert(b"&a<!-- akd--sj\n <!-- \t\0f--ajk--df->lafj -->mp", b"&amp");
  t.insert(
    b"<script><!-- akd--sj\n <!-- \t\0f--ajk--df->lafj --></script>",
    b"<script><!-- akd--sj\n <!-- \t\0f--ajk--df->lafj --></script>",
  );

  // processing instructions
  t.insert(b"<?php hello??? >>  ?>", b"<?php hello??? >>  ?>");
  t.insert(b"av<?xml 1.0 ?>g", b"av<?xml 1.0 ?>g");

  // self closing svg
  t.insert(
    b"<a><svg viewBox=\"0 0 700 100\" /></a><footer></footer>",
    b"<a><svg viewbox=\"0 0 700 100\"/></a><footer></footer>",
  );
  t.insert(
    b"<a><svg viewBox=\"0 0 700 100\"></svg></a><footer></footer>",
    b"<a><svg viewbox=\"0 0 700 100\"></svg></a><footer></footer>",
  );

  t
}

pub fn create_common_css_test_data() -> HashMap<&'static [u8], &'static [u8]> {
  let mut t = HashMap::<&'static [u8], &'static [u8]>::new();

  // style element minification
  t.insert(
    b"<style>div { color: yellow }</style>",
    b"<style>div{color:#ff0}</style>",
  );

  t
}

pub fn create_common_js_test_data() -> HashMap<&'static [u8], &'static [u8]> {
  let mut t = HashMap::<&'static [u8], &'static [u8]>::new();

  // js minification
  t.insert(b"<script></script>", b"<script></script>");
  t.insert(b"<script> </script>", b"<script></script>");
  t.insert(b"<script>let a = 1;</script>", b"<script>let a=1;</script>");
  t.insert(
    b"<script type=text/javascript>let a = 1;</script>",
    b"<script>let a=1;</script>",
  );
  t.insert(
    br#"
        <script>let a = 1;</script>
        <script>let b = 2;</script>
    "#,
    b"<script>let a=1;</script><script>let b=2;</script>",
  );
  t.insert(
    b"<scRIPt type=text/plain>   alert(1.00000);   </scripT>",
    b"<script type=text/plain>   alert(1.00000);   </script>",
  );
  t.insert(
    br#"
        <script>
            // This is a comment.
            let a = 1;
        </script>
    "#,
    b"<script>// This is a comment.\nlet a=1;</script>",
  );

  // js minification unintentional closing tag
  /* TODO Reenable once unintentional script closing tag escaping is implemented in minify-js.
  t.insert(
      br#"<script>let a = "</" + "script>";</script>"#,
      br#"<script>let a="<\/script>";</script>"#,
  );
  t.insert(
      br#"<script>let a = "</S" + "cRiPT>";</script>"#,
      br#"<script>let a="<\/ScRiPT>";</script>"#,
  );
  t.insert(
      br#"<script>let a = "\u003c/script>";</script>"#,
      br#"<script>let a="<\/script>";</script>"#,
  );
  t.insert(
      br#"<script>let a = "\u003c/scrIPt>";</script>"#,
      br#"<script>let a="<\/scrIPt>";</script>"#,
  );
  */

  t
}

pub fn create_common_noncompliant_test_data() -> HashMap<&'static [u8], &'static [u8]> {
  let mut t = HashMap::<&'static [u8], &'static [u8]>::new();

  // attr unquoted value minification
  t.insert(b"<a b==></a>", b"<a b==></a>");
  t.insert(b"<a b=`'\"<<==/`/></a>", b"<a b=`'\"<<==/`/></a>");

  t
}
