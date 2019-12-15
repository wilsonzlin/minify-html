use ::phf::{phf_set, Set};

static CONTENT_FIRST_TAGS: Set<&'static str> = phf_set! {
	"dd",
	"details",
	"dt",
	"iframe",
	"label",
	"li",
	"noscript",
	"output",
	"progress",
	"slot",
	"td",
	"template",
	"th",
};
