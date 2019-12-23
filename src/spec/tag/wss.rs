// "WSS" stands for whitespace-sensitive.
use ::phf::{phf_set, Set};

pub static WSS_TAGS: Set<&'static [u8]> = phf_set! {
	b"code",
	b"pre",
};
