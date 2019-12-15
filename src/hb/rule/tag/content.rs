use ::phf::{phf_set, Set};

static CONTENT_TAGS: Set<&'static str> = phf_set! {
	"address",
	"audio",
	"button",
	"canvas",
	"caption",
	"figcaption",
	"h1",
	"h2",
	"h3",
	"h4",
	"h5",
	"h6",
	"legend",
	"meter",
	"object",
	"option",
	"p",
	"summary", // Can also contain a heading.
	"textarea",
	"video",
};
