use ::phf::{phf_set, Set};

// Does not include whitespace, which is also disallowed.
static ATTR_VAL_UNQUOTED_NON_WHITESPACE_DISALLOWED: Set<char> = phf_set! {
	'"',
	'\'',
	'`',
	'=',
	'<',
	'>',
};

fn is_valid_attr_value_unquoted_char(c: char) -> bool {
    not(ATTR_VAL_UNQUOTED_NON_WHITESPACE_DISALLOWED.has(c) || c.is_ascii_whitespace())
}
