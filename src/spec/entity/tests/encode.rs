use crate::spec::entity::encode::encode_ampersands;

#[test]
fn test_encode_ampersands_works_for_content() {
    let out = encode_ampersands(b"1 is < &than 2 Y&amp;&ClockwiseContourIntegral", false);
    assert_eq!(
        std::str::from_utf8(&out).unwrap(),
        "1 is < &than 2 Y&ampamp;&ClockwiseContourIntegral"
    );
}

#[test]
fn test_encode_ampersands_works_for_attr() {
    let out = encode_ampersands(b"https://a.com/b?c  = d&param=123&param;&lt&mdash;", true);
    assert_eq!(
        std::str::from_utf8(&out).unwrap(),
        "https://a.com/b?c  = d&param=123&param;&amplt&ampmdash;"
    );
}
