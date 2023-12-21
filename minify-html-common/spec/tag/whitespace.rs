use crate::common::spec::tag::ns::Namespace;
use rustc_hash::FxHashMap;

use lazy_static::lazy_static;

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

static DEFAULT_HTML: &WhitespaceMinification = &WhitespaceMinification {
    collapse: true,
    destroy_whole: false,
    trim: false,
};

// SVG 2 spec requires unknown elements to be treated like <g>:
// https://www.w3.org/TR/SVG2/struct.html#UnknownElement.
static DEFAULT_SVG: &WhitespaceMinification = &WhitespaceMinification {
    collapse: true,
    destroy_whole: true,
    trim: true,
};

lazy_static! {
    static ref HTML_TAG_WHITESPACE_MINIFICATION: FxHashMap<&'static [u8], &'static WhitespaceMinification> = {
        let mut m = FxHashMap::<&'static [u8], &'static WhitespaceMinification>::default();
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
        m.insert(b"textarea", WHITESPACE_SENSITIVE);
        m.insert(b"code", WHITESPACE_SENSITIVE);
        m.insert(b"pre", WHITESPACE_SENSITIVE);

        m
    };

    static ref SVG_TAG_WHITESPACE_MINIFICATION: FxHashMap<&'static [u8], &'static WhitespaceMinification> = {
      let mut m = FxHashMap::<&'static [u8], &'static WhitespaceMinification>::default();

      // Content tags.
      m.insert(b"desc", CONTENT);
      m.insert(b"text", CONTENT);
      m.insert(b"title", CONTENT);

      // Formatting tags.
      m.insert(b"a", FORMATTING);
      m.insert(b"altGlyph", FORMATTING);
      m.insert(b"tspan", FORMATTING);
      m.insert(b"textPath", FORMATTING);
      m.insert(b"tref", FORMATTING);

      // Layout tags.
      m.insert(b"altGlyphDef", LAYOUT);
      m.insert(b"altGlyphItem", LAYOUT);
      m.insert(b"animate", LAYOUT);
      m.insert(b"animateColor", LAYOUT);
      m.insert(b"animateMotion", LAYOUT);
      m.insert(b"animateTransform", LAYOUT);
      m.insert(b"circle", LAYOUT);
      m.insert(b"clipPath", LAYOUT);
      m.insert(b"cursor", LAYOUT);
      m.insert(b"defs", LAYOUT);
      m.insert(b"discard", LAYOUT);
      m.insert(b"ellipse", LAYOUT);
      m.insert(b"feBlend", LAYOUT);
      m.insert(b"feColorMatrix", LAYOUT);
      m.insert(b"feComponentTransfer", LAYOUT);
      m.insert(b"feComposite", LAYOUT);
      m.insert(b"feConvolveMatrix", LAYOUT);
      m.insert(b"feDiffuseLighting", LAYOUT);
      m.insert(b"feDisplacementMap", LAYOUT);
      m.insert(b"feDistantLight", LAYOUT);
      m.insert(b"feDropShadow", LAYOUT);
      m.insert(b"feFlood", LAYOUT);
      m.insert(b"feFuncA", LAYOUT);
      m.insert(b"feFuncB", LAYOUT);
      m.insert(b"feFuncG", LAYOUT);
      m.insert(b"feFuncR", LAYOUT);
      m.insert(b"feGaussianBlur", LAYOUT);
      m.insert(b"feImage", LAYOUT);
      m.insert(b"feMerge", LAYOUT);
      m.insert(b"feMergeNode", LAYOUT);
      m.insert(b"feMorphology", LAYOUT);
      m.insert(b"feOffset", LAYOUT);
      m.insert(b"fePointLight", LAYOUT);
      m.insert(b"feSpecularLighting", LAYOUT);
      m.insert(b"feSpotLight", LAYOUT);
      m.insert(b"feTile", LAYOUT);
      m.insert(b"feTurbulence", LAYOUT);
      m.insert(b"filter", LAYOUT);
      m.insert(b"font-face-format", LAYOUT);
      m.insert(b"font-face-name", LAYOUT);
      m.insert(b"font-face-src", LAYOUT);
      m.insert(b"font-face-uri", LAYOUT);
      m.insert(b"font-face", LAYOUT);
      m.insert(b"font", LAYOUT);
      m.insert(b"foreignObject", LAYOUT);
      m.insert(b"g", LAYOUT);
      m.insert(b"glyph", LAYOUT);
      m.insert(b"glyphRef", LAYOUT);
      m.insert(b"hatch", LAYOUT);
      m.insert(b"hatchpath", LAYOUT);
      m.insert(b"hkern", LAYOUT);
      m.insert(b"image", LAYOUT);
      m.insert(b"line", LAYOUT);
      m.insert(b"linearGradient", LAYOUT);
      m.insert(b"marker", LAYOUT);
      m.insert(b"mask", LAYOUT);
      m.insert(b"mesh", LAYOUT);
      m.insert(b"meshgradient", LAYOUT);
      m.insert(b"meshpatch", LAYOUT);
      m.insert(b"meshrow", LAYOUT);
      m.insert(b"metadata", LAYOUT);
      m.insert(b"missing-glyph", LAYOUT);
      m.insert(b"mpath", LAYOUT);
      m.insert(b"path", LAYOUT);
      m.insert(b"pattern", LAYOUT);
      m.insert(b"polygon", LAYOUT);
      m.insert(b"polyline", LAYOUT);
      m.insert(b"radialGradient", LAYOUT);
      m.insert(b"rect", LAYOUT);
      m.insert(b"set", LAYOUT);
      m.insert(b"solidcolor", LAYOUT);
      m.insert(b"stop", LAYOUT);
      m.insert(b"svg", LAYOUT);
      m.insert(b"switch", LAYOUT);
      m.insert(b"symbol", LAYOUT);
      m.insert(b"use", LAYOUT);
      m.insert(b"view", LAYOUT);
      m.insert(b"vkern", LAYOUT);

      m
    };
}

pub fn get_whitespace_minification_for_tag(
    ns: Namespace,
    // Use empty slice if root.
    tag_name: &[u8],
    descendant_of_pre: bool,
) -> &'static WhitespaceMinification {
    match ns {
        Namespace::Html => {
            if descendant_of_pre {
                WHITESPACE_SENSITIVE
            } else if tag_name.is_empty() {
                ROOT
            } else {
                HTML_TAG_WHITESPACE_MINIFICATION
                    .get(tag_name)
                    .unwrap_or(&DEFAULT_HTML)
            }
        }
        Namespace::Svg => SVG_TAG_WHITESPACE_MINIFICATION
            .get(tag_name)
            .unwrap_or(&DEFAULT_SVG),
    }
}
