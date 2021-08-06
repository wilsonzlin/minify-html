use crate::spec::entity::encode::encode_ampersands;

#[test]
fn test_encode_ampersands_works_for_content() {
    let out = encode_ampersands(b"1 is < than 2 <? </", false);
    assert_eq!(out, b"1 is < than 2 &LT? &LT/".to_vec());
}
