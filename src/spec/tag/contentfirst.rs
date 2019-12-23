use ::phf::{phf_set, Set};

pub static CONTENT_FIRST_TAGS: Set<&'static [u8]> = phf_set! {
	b"dd",
	b"details",
	b"dt",
	b"iframe",
	b"label",
	b"li",
	b"noscript",
	b"output",
	b"progress",
	b"slot",
	b"td",
	b"template",
	b"th",
};
