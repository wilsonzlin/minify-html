use ::phf::{phf_set, Set};

pub static CONTENT_TAGS: Set<&'static [u8]> = phf_set! {
	b"address",
	b"audio",
	b"button",
	b"canvas",
	b"caption",
	b"figcaption",
	b"h1",
	b"h2",
	b"h3",
	b"h4",
	b"h5",
	b"h6",
	b"legend",
	b"meter",
	b"object",
	b"option",
	b"p",
	b"summary", // Can also contain a heading.
	b"textarea",
	b"video",
};
