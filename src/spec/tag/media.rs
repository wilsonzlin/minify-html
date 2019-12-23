use ::phf::{phf_set, Set};

pub static MEDIA_TAGS: Set<&'static [u8]> = phf_set! {
	b"audio",
	b"video",
};
