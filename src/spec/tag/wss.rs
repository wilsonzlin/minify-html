use ::phf::{phf_set, Set};

// "WSS" stands for whitespace-sensitive.
pub static WSS_TAGS: Set<&'static [u8]> = phf_set! {
	b"code",
	b"pre",
};
