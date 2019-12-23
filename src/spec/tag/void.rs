use ::phf::{phf_set, Set};

pub static VOID_TAGS: Set<&'static [u8]> = phf_set! {
	b"area",
	b"base",
	b"br",
	b"col",
	b"embed",
	b"hr",
	b"img",
	b"input",
	b"keygen",
	b"link",
	b"meta",
	b"param",
	b"source",
	b"track",
	b"wbr",
};
