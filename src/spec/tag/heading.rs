use ::phf::{phf_set, Set};

pub static HEADING_TAGS: Set<&'static [u8]> = phf_set! {
	b"hgroup",
	b"h1",
	b"h2",
	b"h3",
	b"h4",
	b"h5",
	b"h6",
};
