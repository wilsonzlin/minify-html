use phf::{Map, phf_map};

pub struct WhitespaceMinification {
    pub collapse: bool,
    pub destroy_whole: bool,
    pub trim: bool,
}

static CONTENT: &WhitespaceMinification = &WhitespaceMinification {
    collapse: true,
    destroy_whole: false,
    trim: true,
};

static CONTENT_FIRST: &WhitespaceMinification = &WhitespaceMinification {
    collapse: true,
    destroy_whole: false,
    trim: true,
};

static FORMATTING: &WhitespaceMinification = &WhitespaceMinification {
    collapse: true,
    destroy_whole: false,
    trim: false,
};

static LAYOUT: &WhitespaceMinification = &WhitespaceMinification {
    collapse: true,
    destroy_whole: true,
    trim: true,
};

static WHITESPACE_SENSITIVE: &WhitespaceMinification = &WhitespaceMinification {
    collapse: false,
    destroy_whole: false,
    trim: false,
};

static ROOT: &WhitespaceMinification = &WhitespaceMinification {
    collapse: true,
    destroy_whole: true,
    trim: true,
};

static DEFAULT: &WhitespaceMinification = &WhitespaceMinification {
    collapse: true,
    destroy_whole: false,
    trim: false,
};

static TAG_WHITESPACE_MINIFICATION: Map<&'static [u8], &'static WhitespaceMinification> = phf_map! {
    // Content tags.
	b"address" => CONTENT,
	b"audio" => CONTENT,
	b"button" => CONTENT,
	b"canvas" => CONTENT,
	b"caption" => CONTENT,
	b"figcaption" => CONTENT,
	b"h1" => CONTENT,
	b"h2" => CONTENT,
	b"h3" => CONTENT,
	b"h4" => CONTENT,
	b"h5" => CONTENT,
	b"h6" => CONTENT,
	b"legend" => CONTENT,
	b"meter" => CONTENT,
	b"object" => CONTENT,
	b"option" => CONTENT,
	b"p" => CONTENT,
	b"summary" => CONTENT,
	b"textarea" => CONTENT,
	b"video" => CONTENT,

	// Content-first tags.
	b"dd" => CONTENT_FIRST,
	b"details" => CONTENT_FIRST,
	b"dt" => CONTENT_FIRST,
	b"iframe" => CONTENT_FIRST,
	b"label" => CONTENT_FIRST,
	b"li" => CONTENT_FIRST,
	b"noscript" => CONTENT_FIRST,
	b"output" => CONTENT_FIRST,
	b"progress" => CONTENT_FIRST,
	b"slot" => CONTENT_FIRST,
	b"td" => CONTENT_FIRST,
	b"template" => CONTENT_FIRST,
	b"th" => CONTENT_FIRST,

	// Formatting tags.
    // Sourced from https://developer.mozilla.org/en-US/docs/Web/HTML/Element#Inline_text_semantics.
    // Differences to tags listed in table at above URL: -br, +del, +ins.
	b"a" => FORMATTING,
	b"abbr" => FORMATTING,
	b"b" => FORMATTING,
	b"bdi" => FORMATTING,
	b"bdo" => FORMATTING,
	b"cite" => FORMATTING,
	b"data" => FORMATTING,
	b"del" => FORMATTING,
	b"dfn" => FORMATTING,
	b"em" => FORMATTING,
	b"i" => FORMATTING,
	b"ins" => FORMATTING,
	b"kbd" => FORMATTING,
	b"mark" => FORMATTING,
	b"q" => FORMATTING,
	b"rp" => FORMATTING,
	b"rt" => FORMATTING,
	b"rtc" => FORMATTING,
	b"ruby" => FORMATTING,
	b"s" => FORMATTING,
	b"samp" => FORMATTING,
	b"small" => FORMATTING,
	b"span" => FORMATTING,
	b"strong" => FORMATTING,
	b"sub" => FORMATTING,
	b"sup" => FORMATTING,
	b"time" => FORMATTING,
	b"u" => FORMATTING,
	b"var" => FORMATTING,
	b"wbr" => FORMATTING,

	// Layout tags.
	b"article" => LAYOUT,
	b"aside" => LAYOUT,
	b"blockquote" => LAYOUT,
	b"body" => LAYOUT,
	b"colgroup" => LAYOUT,
	b"datalist" => LAYOUT,
	b"dialog" => LAYOUT,
	b"div" => LAYOUT,
	b"dl" => LAYOUT,
	b"fieldset" => LAYOUT,
	b"figure" => LAYOUT,
	b"footer" => LAYOUT,
	b"form" => LAYOUT,
	b"head" => LAYOUT,
	b"header" => LAYOUT,
	b"hgroup" => LAYOUT,
	b"html" => LAYOUT,
	b"main" => LAYOUT,
	b"map" => LAYOUT,
	b"menu" => LAYOUT,
	b"nav" => LAYOUT,
	b"ol" => LAYOUT,
	b"optgroup" => LAYOUT,
	b"picture" => LAYOUT,
	b"section" => LAYOUT,
	b"select" => LAYOUT,
	b"table" => LAYOUT,
	b"tbody" => LAYOUT,
	b"tfoot" => LAYOUT,
	b"thead" => LAYOUT,
	b"tr" => LAYOUT,
	b"ul" => LAYOUT,

	// Whitespace-sensitive tags.
	b"code" => WHITESPACE_SENSITIVE,
	b"pre" => WHITESPACE_SENSITIVE,
};

pub fn get_whitespace_minification_for_tag(tag_name: Option<&[u8]>) -> &'static WhitespaceMinification {
    match tag_name {
        Some(n) => TAG_WHITESPACE_MINIFICATION.get(n).unwrap_or(&DEFAULT),
        None => ROOT,
    }
}
