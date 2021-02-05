use lazy_static::lazy_static;
use std::collections::HashMap;

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

lazy_static! {
    static ref TAG_WHITESPACE_MINIFICATION: HashMap<&'static [u8], &'static WhitespaceMinification> = {
        let mut m = HashMap::<&'static [u8], &'static WhitespaceMinification>::new();
        // Content tags.
        m.insert(b"address", CONTENT);
        m.insert(b"audio", CONTENT);
        m.insert(b"button", CONTENT);
        m.insert(b"canvas", CONTENT);
        m.insert(b"caption", CONTENT);
        m.insert(b"figcaption", CONTENT);
        m.insert(b"h1", CONTENT);
        m.insert(b"h2", CONTENT);
        m.insert(b"h3", CONTENT);
        m.insert(b"h4", CONTENT);
        m.insert(b"h5", CONTENT);
        m.insert(b"h6", CONTENT);
        m.insert(b"legend", CONTENT);
        m.insert(b"meter", CONTENT);
        m.insert(b"object", CONTENT);
        m.insert(b"option", CONTENT);
        m.insert(b"p", CONTENT);
        m.insert(b"summary", CONTENT);
        m.insert(b"textarea", CONTENT);
        m.insert(b"video", CONTENT);

        // Content-first tags.
        m.insert(b"dd", CONTENT_FIRST);
        m.insert(b"details", CONTENT_FIRST);
        m.insert(b"dt", CONTENT_FIRST);
        m.insert(b"iframe", CONTENT_FIRST);
        m.insert(b"label", CONTENT_FIRST);
        m.insert(b"li", CONTENT_FIRST);
        m.insert(b"noscript", CONTENT_FIRST);
        m.insert(b"output", CONTENT_FIRST);
        m.insert(b"progress", CONTENT_FIRST);
        m.insert(b"slot", CONTENT_FIRST);
        m.insert(b"td", CONTENT_FIRST);
        m.insert(b"template", CONTENT_FIRST);
        m.insert(b"th", CONTENT_FIRST);

        // Formatting tags.
        // Sourced from https://developer.mozilla.org/en-US/docs/Web/HTML/Element#Inline_text_semantics.
        // Differences to tags listed in table at above URL: -br, +del, +ins.
        m.insert(b"a", FORMATTING);
        m.insert(b"abbr", FORMATTING);
        m.insert(b"b", FORMATTING);
        m.insert(b"bdi", FORMATTING);
        m.insert(b"bdo", FORMATTING);
        m.insert(b"cite", FORMATTING);
        m.insert(b"data", FORMATTING);
        m.insert(b"del", FORMATTING);
        m.insert(b"dfn", FORMATTING);
        m.insert(b"em", FORMATTING);
        m.insert(b"i", FORMATTING);
        m.insert(b"ins", FORMATTING);
        m.insert(b"kbd", FORMATTING);
        m.insert(b"mark", FORMATTING);
        m.insert(b"q", FORMATTING);
        m.insert(b"rp", FORMATTING);
        m.insert(b"rt", FORMATTING);
        m.insert(b"rtc", FORMATTING);
        m.insert(b"ruby", FORMATTING);
        m.insert(b"s", FORMATTING);
        m.insert(b"samp", FORMATTING);
        m.insert(b"small", FORMATTING);
        m.insert(b"span", FORMATTING);
        m.insert(b"strong", FORMATTING);
        m.insert(b"sub", FORMATTING);
        m.insert(b"sup", FORMATTING);
        m.insert(b"time", FORMATTING);
        m.insert(b"u", FORMATTING);
        m.insert(b"var", FORMATTING);
        m.insert(b"wbr", FORMATTING);

        // Layout tags.
        m.insert(b"article", LAYOUT);
        m.insert(b"aside", LAYOUT);
        m.insert(b"blockquote", LAYOUT);
        m.insert(b"body", LAYOUT);
        m.insert(b"colgroup", LAYOUT);
        m.insert(b"datalist", LAYOUT);
        m.insert(b"dialog", LAYOUT);
        m.insert(b"div", LAYOUT);
        m.insert(b"dl", LAYOUT);
        m.insert(b"fieldset", LAYOUT);
        m.insert(b"figure", LAYOUT);
        m.insert(b"footer", LAYOUT);
        m.insert(b"form", LAYOUT);
        m.insert(b"head", LAYOUT);
        m.insert(b"header", LAYOUT);
        m.insert(b"hgroup", LAYOUT);
        m.insert(b"html", LAYOUT);
        m.insert(b"main", LAYOUT);
        m.insert(b"map", LAYOUT);
        m.insert(b"menu", LAYOUT);
        m.insert(b"nav", LAYOUT);
        m.insert(b"ol", LAYOUT);
        m.insert(b"optgroup", LAYOUT);
        m.insert(b"picture", LAYOUT);
        m.insert(b"section", LAYOUT);
        m.insert(b"select", LAYOUT);
        m.insert(b"table", LAYOUT);
        m.insert(b"tbody", LAYOUT);
        m.insert(b"tfoot", LAYOUT);
        m.insert(b"thead", LAYOUT);
        m.insert(b"tr", LAYOUT);
        m.insert(b"ul", LAYOUT);

        // Whitespace-sensitive tags.
        m.insert(b"code", WHITESPACE_SENSITIVE);
        m.insert(b"pre", WHITESPACE_SENSITIVE);

        m
    };
}

#[inline(always)]
pub fn get_whitespace_minification_for_tag(tag_name: Option<&[u8]>, descendant_of_pre: bool) -> &'static WhitespaceMinification {
    if descendant_of_pre {
        WHITESPACE_SENSITIVE
    } else {
        match tag_name {
            Some(n) => TAG_WHITESPACE_MINIFICATION.get(n).unwrap_or(&DEFAULT),
            None => ROOT,
        }
    }
}
