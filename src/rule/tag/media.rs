use ::phf::{phf_set, Set};

static MEDIA_TAGS: Set<&'static str> = phf_set! {
	"audio",
	"video",
};
