#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

// Sourced from https://developer.mozilla.org/en-US/docs/Web/SVG/Element at
// 2018-08-04T03:50:00Z

static hb_set_tag_names* hb_rule_tag_svg_set;

void hb_rule_tag_svg_add_elems(hb_set_tag_names* set) {
    hb_set_tag_names_add_whole_literal(set, "a");
    hb_set_tag_names_add_whole_literal(set, "altGlyph");
    hb_set_tag_names_add_whole_literal(set, "altGlyphDef");
    hb_set_tag_names_add_whole_literal(set, "altGlyphItem");
    hb_set_tag_names_add_whole_literal(set, "animate");
    hb_set_tag_names_add_whole_literal(set, "animateColor");
    hb_set_tag_names_add_whole_literal(set, "animateMotion");
    hb_set_tag_names_add_whole_literal(set, "animateTransform");
    hb_set_tag_names_add_whole_literal(set, "circle");
    hb_set_tag_names_add_whole_literal(set, "clipPath");
    hb_set_tag_names_add_whole_literal(set, "color-profile");
    hb_set_tag_names_add_whole_literal(set, "cursor");
    hb_set_tag_names_add_whole_literal(set, "defs");
    hb_set_tag_names_add_whole_literal(set, "desc");
    hb_set_tag_names_add_whole_literal(set, "discard");
    hb_set_tag_names_add_whole_literal(set, "ellipse");
    hb_set_tag_names_add_whole_literal(set, "feBlend");
    hb_set_tag_names_add_whole_literal(set, "feColorMatrix");
    hb_set_tag_names_add_whole_literal(set, "feComponentTransfer");
    hb_set_tag_names_add_whole_literal(set, "feComposite");
    hb_set_tag_names_add_whole_literal(set, "feConvolveMatrix");
    hb_set_tag_names_add_whole_literal(set, "feDiffuseLighting");
    hb_set_tag_names_add_whole_literal(set, "feDisplacementMap");
    hb_set_tag_names_add_whole_literal(set, "feDistantLight");
    hb_set_tag_names_add_whole_literal(set, "feDropShadow");
    hb_set_tag_names_add_whole_literal(set, "feFlood");
    hb_set_tag_names_add_whole_literal(set, "feFuncA");
    hb_set_tag_names_add_whole_literal(set, "feFuncB");
    hb_set_tag_names_add_whole_literal(set, "feFuncG");
    hb_set_tag_names_add_whole_literal(set, "feFuncR");
    hb_set_tag_names_add_whole_literal(set, "feGaussianBlur");
    hb_set_tag_names_add_whole_literal(set, "feImage");
    hb_set_tag_names_add_whole_literal(set, "feMerge");
    hb_set_tag_names_add_whole_literal(set, "feMergeNode");
    hb_set_tag_names_add_whole_literal(set, "feMorphology");
    hb_set_tag_names_add_whole_literal(set, "feOffset");
    hb_set_tag_names_add_whole_literal(set, "fePointLight");
    hb_set_tag_names_add_whole_literal(set, "feSpecularLighting");
    hb_set_tag_names_add_whole_literal(set, "feSpotLight");
    hb_set_tag_names_add_whole_literal(set, "feTile");
    hb_set_tag_names_add_whole_literal(set, "feTurbulence");
    hb_set_tag_names_add_whole_literal(set, "filter");
    hb_set_tag_names_add_whole_literal(set, "font-face-format");
    hb_set_tag_names_add_whole_literal(set, "font-face-name");
    hb_set_tag_names_add_whole_literal(set, "font-face-src");
    hb_set_tag_names_add_whole_literal(set, "font-face-uri");
    hb_set_tag_names_add_whole_literal(set, "font-face");
    hb_set_tag_names_add_whole_literal(set, "font");
    hb_set_tag_names_add_whole_literal(set, "foreignObject");
    hb_set_tag_names_add_whole_literal(set, "g");
    hb_set_tag_names_add_whole_literal(set, "glyph");
    hb_set_tag_names_add_whole_literal(set, "glyphRef");
    hb_set_tag_names_add_whole_literal(set, "hatch");
    hb_set_tag_names_add_whole_literal(set, "hatchpath");
    hb_set_tag_names_add_whole_literal(set, "hkern");
    hb_set_tag_names_add_whole_literal(set, "image");
    hb_set_tag_names_add_whole_literal(set, "line");
    hb_set_tag_names_add_whole_literal(set, "linearGradient");
    hb_set_tag_names_add_whole_literal(set, "marker");
    hb_set_tag_names_add_whole_literal(set, "mask");
    hb_set_tag_names_add_whole_literal(set, "mesh");
    hb_set_tag_names_add_whole_literal(set, "meshgradient");
    hb_set_tag_names_add_whole_literal(set, "meshpatch");
    hb_set_tag_names_add_whole_literal(set, "meshrow");
    hb_set_tag_names_add_whole_literal(set, "metadata");
    hb_set_tag_names_add_whole_literal(set, "missing-glyph");
    hb_set_tag_names_add_whole_literal(set, "mpath");
    hb_set_tag_names_add_whole_literal(set, "path");
    hb_set_tag_names_add_whole_literal(set, "pattern");
    hb_set_tag_names_add_whole_literal(set, "polygon");
    hb_set_tag_names_add_whole_literal(set, "polyline");
    hb_set_tag_names_add_whole_literal(set, "radialGradient");
    hb_set_tag_names_add_whole_literal(set, "rect");
    hb_set_tag_names_add_whole_literal(set, "script");
    hb_set_tag_names_add_whole_literal(set, "set");
    hb_set_tag_names_add_whole_literal(set, "solidcolor");
    hb_set_tag_names_add_whole_literal(set, "stop");
    hb_set_tag_names_add_whole_literal(set, "style");
    hb_set_tag_names_add_whole_literal(set, "svg");
    hb_set_tag_names_add_whole_literal(set, "switch");
    hb_set_tag_names_add_whole_literal(set, "symbol");
    hb_set_tag_names_add_whole_literal(set, "text");
    hb_set_tag_names_add_whole_literal(set, "textPath");
    hb_set_tag_names_add_whole_literal(set, "title");
    hb_set_tag_names_add_whole_literal(set, "tref");
    hb_set_tag_names_add_whole_literal(set, "tspan");
    hb_set_tag_names_add_whole_literal(set, "unknown");
    hb_set_tag_names_add_whole_literal(set, "use");
    hb_set_tag_names_add_whole_literal(set, "view");
    hb_set_tag_names_add_whole_literal(set, "vkern");
}

void hb_rule_tag_svg_init(void) {
    hb_rule_tag_svg_set = hb_set_tag_names_create();
    hb_rule_tag_svg_add_elems(hb_rule_tag_svg_set);
}

bool hb_rule_tag_svg_check(nh_view_str* tag) {
    return hb_set_tag_names_has(hb_rule_tag_svg_set, tag);
}
