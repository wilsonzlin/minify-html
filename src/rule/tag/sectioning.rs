use ::phf::{phf_set, Set};

static SECTIONING_TAGS: Set<&'static str> = phf_set! {
    // Also used by layout tags.
	"article",
	"aside",
	"nav",
	"section",
};
