use ::phf::{phf_set, Set};

static HEADING_TAGS: Set<&'static str> = phf_set! {
	"hgroup",
	"h1",
	"h2",
	"h3",
	"h4",
	"h5",
	"h6",
};
