use ::phf::{phf_set, Set};

// Does not include SVG tags.
pub static SPECIFIC_HTML_TAGS: Set<&'static [u8]> = phf_set! {
	b"area",
	b"base",
	b"br",
	b"code", // Reason: unlikely to want to minify.
	b"col",
	b"embed",
	b"hr",
	b"img",
	b"input",
	b"param",
	b"pre", // Reason: unlikely to want to minify.
	b"script",
	b"source",
	b"track",
};
