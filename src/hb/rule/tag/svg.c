#pragma once

#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

// Sourced from https://developer.mozilla.org/en-US/docs/Web/SVG/Element at
// 2018-08-04T03:50:00Z

static nh_set_str* hb_rule_tag_svg_set;

void hb_rule_tag_svg_add_elems(nh_set_str* set) {
    nh_set_str_add(set, "a");
    nh_set_str_add(set, "altGlyph");
    nh_set_str_add(set, "altGlyphDef");
    nh_set_str_add(set, "altGlyphItem");
    nh_set_str_add(set, "animate");
    nh_set_str_add(set, "animateColor");
    nh_set_str_add(set, "animateMotion");
    nh_set_str_add(set, "animateTransform");
    nh_set_str_add(set, "circle");
    nh_set_str_add(set, "clipPath");
    nh_set_str_add(set, "color-profile");
    nh_set_str_add(set, "cursor");
    nh_set_str_add(set, "defs");
    nh_set_str_add(set, "desc");
    nh_set_str_add(set, "discard");
    nh_set_str_add(set, "ellipse");
    nh_set_str_add(set, "feBlend");
    nh_set_str_add(set, "feColorMatrix");
    nh_set_str_add(set, "feComponentTransfer");
    nh_set_str_add(set, "feComposite");
    nh_set_str_add(set, "feConvolveMatrix");
    nh_set_str_add(set, "feDiffuseLighting");
    nh_set_str_add(set, "feDisplacementMap");
    nh_set_str_add(set, "feDistantLight");
    nh_set_str_add(set, "feDropShadow");
    nh_set_str_add(set, "feFlood");
    nh_set_str_add(set, "feFuncA");
    nh_set_str_add(set, "feFuncB");
    nh_set_str_add(set, "feFuncG");
    nh_set_str_add(set, "feFuncR");
    nh_set_str_add(set, "feGaussianBlur");
    nh_set_str_add(set, "feImage");
    nh_set_str_add(set, "feMerge");
    nh_set_str_add(set, "feMergeNode");
    nh_set_str_add(set, "feMorphology");
    nh_set_str_add(set, "feOffset");
    nh_set_str_add(set, "fePointLight");
    nh_set_str_add(set, "feSpecularLighting");
    nh_set_str_add(set, "feSpotLight");
    nh_set_str_add(set, "feTile");
    nh_set_str_add(set, "feTurbulence");
    nh_set_str_add(set, "filter");
    nh_set_str_add(set, "font-face-format");
    nh_set_str_add(set, "font-face-name");
    nh_set_str_add(set, "font-face-src");
    nh_set_str_add(set, "font-face-uri");
    nh_set_str_add(set, "font-face");
    nh_set_str_add(set, "font");
    nh_set_str_add(set, "foreignObject");
    nh_set_str_add(set, "g");
    nh_set_str_add(set, "glyph");
    nh_set_str_add(set, "glyphRef");
    nh_set_str_add(set, "hatch");
    nh_set_str_add(set, "hatchpath");
    nh_set_str_add(set, "hkern");
    nh_set_str_add(set, "image");
    nh_set_str_add(set, "line");
    nh_set_str_add(set, "linearGradient");
    nh_set_str_add(set, "marker");
    nh_set_str_add(set, "mask");
    nh_set_str_add(set, "mesh");
    nh_set_str_add(set, "meshgradient");
    nh_set_str_add(set, "meshpatch");
    nh_set_str_add(set, "meshrow");
    nh_set_str_add(set, "metadata");
    nh_set_str_add(set, "missing-glyph");
    nh_set_str_add(set, "mpath");
    nh_set_str_add(set, "path");
    nh_set_str_add(set, "pattern");
    nh_set_str_add(set, "polygon");
    nh_set_str_add(set, "polyline");
    nh_set_str_add(set, "radialGradient");
    nh_set_str_add(set, "rect");
    nh_set_str_add(set, "script");
    nh_set_str_add(set, "set");
    nh_set_str_add(set, "solidcolor");
    nh_set_str_add(set, "stop");
    nh_set_str_add(set, "style");
    nh_set_str_add(set, "svg");
    nh_set_str_add(set, "switch");
    nh_set_str_add(set, "symbol");
    nh_set_str_add(set, "text");
    nh_set_str_add(set, "textPath");
    nh_set_str_add(set, "title");
    nh_set_str_add(set, "tref");
    nh_set_str_add(set, "tspan");
    nh_set_str_add(set, "unknown");
    nh_set_str_add(set, "use");
    nh_set_str_add(set, "view");
    nh_set_str_add(set, "vkern");
}

void hb_rule_tag_svg_init(void) {
    hb_rule_tag_svg_set = nh_set_str_create();
    hb_rule_tag_svg_add_elems(hb_rule_tag_svg_set);
}

bool hb_rule_tag_svg_check(hb_rune* tag) {
    return nh_set_str_has(hb_rule_tag_svg_set, tag);
}
