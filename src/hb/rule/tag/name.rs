fn is_valid_tag_name_char(c: char) -> bool {
    c.is_ascii_alphabetic() || c.is_ascii_digit() || c == ':' || c == '-'
}
