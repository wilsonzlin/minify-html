use crate::entity::encode::encode_entities;

#[test]
fn test_encode_entities_encodes_ampersands_when_they_form_valid_entities() {
    let out = encode_entities(b"1 is < &than 2 Y&amp;&ClockwiseContourIntegral", false);
    assert_eq!(
        std::str::from_utf8(&out).unwrap(),
        "1 is < &than 2 Y&ampamp;&ClockwiseContourIntegral"
    );
}

#[test]
fn test_encode_entities_does_not_encode_valid_named_entities_inside_an_attr_value_if_they_do_not_end_with_a_semicolon_but_are_followed_by_an_alphanumeric_or_equals_character(
) {
    let out = encode_entities(b"https://a.com/b?c  = d&param=123&param;&lt&mdash;", true);
    assert_eq!(
        std::str::from_utf8(&out).unwrap(),
        "https://a.com/b?c  = d&param=123&param;&amplt&ampmdash;"
    );
}

#[test]
fn test_encode_entities_encodes_utf8_sequences_that_are_shorter_encoded() {
    let out = encode_entities("\u{226A}\u{20D2}".as_bytes(), false);
    assert_eq!(std::str::from_utf8(&out).unwrap(), "&nLt;");
}
