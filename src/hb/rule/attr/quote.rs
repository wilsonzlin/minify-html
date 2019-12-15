use ::phf::{phf_set, Set};

static ATTR_QUOTE: Set<char> = phf_set! {
	// Backtick is not a valid quote character according to
	// https://html.spec.whatwg.org/multipage/introduction.html#intro-early-example
	'\'',
	'"',
};
