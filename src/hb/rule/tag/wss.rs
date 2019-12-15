// "WSS" stands for whitespace-sensitive.
use ::phf::{phf_set, Set};

static WSS_TAGS: Set<&'static str> = phf_set! {
	"code",
	"pre",
};
