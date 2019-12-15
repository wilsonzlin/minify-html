use ::phf::{phf_set, Set};

// Does not include SVG tags.
static SPECIFIC_HTML_TAGS: Set<&'static str> = phf_set! {
	"area",
	"base",
	"br",
	"code", // Reason: unlikely to want to minify.
	"col",
	"embed",
	"hr",
	"img",
	"input",
	"param",
	"pre", // Reason: unlikely to want to minify.
	"script",
	"source",
	"track",
}
