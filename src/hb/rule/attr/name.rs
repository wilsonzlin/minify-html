use ::phf::{phf_set, Set};

// Does not include control characters, which are also not allowed.
static ATTR_NAME_NON_CONTROL_DISALLOWED: Set<char> = phf_set! {
	' ',
	'"',
	'\'',
	'>',
	'/',
	'=',
	// NOTE: Unicode noncharacters not tested.
	// (https://html.spec.whatwg.org/multipage/syntax.html#syntax-attribute-name)
};

fn is_valid_attr_name_char(c: char) -> bool {
    not (ATTR_NAME_NON_CONTROL_DISALLOWED.has(c) || c.is_ascii_control())
}
