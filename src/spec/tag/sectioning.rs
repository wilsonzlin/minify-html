use ::phf::{phf_set, Set};

pub static SECTIONING_TAGS: Set<&'static [u8]> = phf_set! {
    // Also used by layout tags.
	b"article",
	b"aside",
	b"nav",
	b"section",
};
