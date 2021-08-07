use crate::minify::attr::{
    encode_unquoted, encode_using_double_quotes, encode_using_single_quotes,
};

#[test]
fn test_encode_using_double_quotes() {
    let min = encode_using_double_quotes(br#"abr"aca"dab &amp&amp;  ""10";""8"$4 a""#);
    assert_eq!(
        min.str(),
        r#""abr&#34aca&#34dab &amp&amp;  &#34&#34;10&#34;;&#34&#34;8&#34$4 a&#34""#,
    );
}

#[test]
fn test_encode_using_single_quotes() {
    let min = encode_using_single_quotes(br#"'abr'aca'dab   &amp&amp;''10';''8'$4 a'"#);
    assert_eq!(
        min.str(),
        r#"'&#39abr&#39aca&#39dab   &amp&amp;&#39&#39;10&#39;;&#39&#39;8&#39$4 a&#39'"#,
    );
}

#[test]
fn test_encode_unquoted() {
    let min = encode_unquoted(br#""123' 'h   0 &amp&amp; ;abbibi "' \ >& 3>;"#, false);
    assert_eq!(
        min.str(),
        r#"&#34;123'&#32'h&#32&#32&#32;0&#32&amp&amp;&#32;;abbibi&#32"'&#32\&#32&GT&&#32;3&GT;;"#,
    );
}
