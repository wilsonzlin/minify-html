use ::phf::{phf_set, Set};

static VOID_TAGS: Set<&'static str> = phf_set! {
	"area",
	"base",
	"br",
	"col",
	"embed",
	"hr",
	"img",
	"input",
	"keygen",
	"link",
	"meta",
	"param",
	"source",
	"track",
	"wbr",
};
