#pragma once

#include <hb/rune.h>
#include <hb/collection.h>
#include <hb/rule.h>

// Sourced from https://developer.mozilla.org/en-US/docs/Web/HTML/Element at
// 2018-07-01T05:55:00Z

static nh_set_str* hb_rule_tag_html_set;

void hb_rule_tag_html_add_elems(nh_set_str* set) {
    nh_set_str_add(set, "a");
    nh_set_str_add(set, "abbr");
    nh_set_str_add(set, "acronym");
    nh_set_str_add(set, "address");
    nh_set_str_add(set, "applet");
    nh_set_str_add(set, "applet");
    nh_set_str_add(set, "area");
    nh_set_str_add(set, "article");
    nh_set_str_add(set, "aside");
    nh_set_str_add(set, "audio");
    nh_set_str_add(set, "b");
    nh_set_str_add(set, "basefont");
    nh_set_str_add(set, "bdi");
    nh_set_str_add(set, "bdo");
    nh_set_str_add(set, "bgsound");
    nh_set_str_add(set, "big");
    nh_set_str_add(set, "blink");
    nh_set_str_add(set, "blockquote");
    nh_set_str_add(set, "body");
    nh_set_str_add(set, "br");
    nh_set_str_add(set, "button");
    nh_set_str_add(set, "canvas");
    nh_set_str_add(set, "caption");
    nh_set_str_add(set, "center");
    nh_set_str_add(set, "cite");
    nh_set_str_add(set, "code");
    nh_set_str_add(set, "col");
    nh_set_str_add(set, "colgroup");
    nh_set_str_add(set, "command");
    nh_set_str_add(set, "content");
    nh_set_str_add(set, "content");
    nh_set_str_add(set, "data");
    nh_set_str_add(set, "datalist");
    nh_set_str_add(set, "dd");
    nh_set_str_add(set, "del");
    nh_set_str_add(set, "details");
    nh_set_str_add(set, "dfn");
    nh_set_str_add(set, "dialog");
    nh_set_str_add(set, "dir");
    nh_set_str_add(set, "dir");
    nh_set_str_add(set, "div");
    nh_set_str_add(set, "dl");
    nh_set_str_add(set, "dt");
    nh_set_str_add(set, "element");
    nh_set_str_add(set, "element");
    nh_set_str_add(set, "em");
    nh_set_str_add(set, "embed");
    nh_set_str_add(set, "fieldset");
    nh_set_str_add(set, "figcaption");
    nh_set_str_add(set, "figure");
    nh_set_str_add(set, "font");
    nh_set_str_add(set, "footer");
    nh_set_str_add(set, "form");
    nh_set_str_add(set, "frame");
    nh_set_str_add(set, "frameset");
    nh_set_str_add(set, "h1");
    nh_set_str_add(set, "h2");
    nh_set_str_add(set, "h3");
    nh_set_str_add(set, "h4");
    nh_set_str_add(set, "h5");
    nh_set_str_add(set, "h6");
    nh_set_str_add(set, "head");
    nh_set_str_add(set, "header");
    nh_set_str_add(set, "hgroup");
    nh_set_str_add(set, "hr");
    nh_set_str_add(set, "html");
    nh_set_str_add(set, "i");
    nh_set_str_add(set, "iframe");
    nh_set_str_add(set, "image");
    nh_set_str_add(set, "img");
    nh_set_str_add(set, "input");
    nh_set_str_add(set, "ins");
    nh_set_str_add(set, "isindex");
    nh_set_str_add(set, "kbd");
    nh_set_str_add(set, "keygen");
    nh_set_str_add(set, "label");
    nh_set_str_add(set, "legend");
    nh_set_str_add(set, "li");
    nh_set_str_add(set, "link");
    nh_set_str_add(set, "listing");
    nh_set_str_add(set, "main");
    nh_set_str_add(set, "map");
    nh_set_str_add(set, "mark");
    nh_set_str_add(set, "marquee");
    nh_set_str_add(set, "menu");
    nh_set_str_add(set, "menuitem");
    nh_set_str_add(set, "menuitem");
    nh_set_str_add(set, "meta");
    nh_set_str_add(set, "meter");
    nh_set_str_add(set, "multicol");
    nh_set_str_add(set, "nav");
    nh_set_str_add(set, "nextid");
    nh_set_str_add(set, "nobr");
    nh_set_str_add(set, "noembed");
    nh_set_str_add(set, "noembed");
    nh_set_str_add(set, "noframes");
    nh_set_str_add(set, "noscript");
    nh_set_str_add(set, "object");
    nh_set_str_add(set, "ol");
    nh_set_str_add(set, "optgroup");
    nh_set_str_add(set, "option");
    nh_set_str_add(set, "output");
    nh_set_str_add(set, "p");
    nh_set_str_add(set, "param");
    nh_set_str_add(set, "picture");
    nh_set_str_add(set, "plaintext");
    nh_set_str_add(set, "pre");
    nh_set_str_add(set, "progress");
    nh_set_str_add(set, "q");
    nh_set_str_add(set, "rp");
    nh_set_str_add(set, "rt");
    nh_set_str_add(set, "rtc");
    nh_set_str_add(set, "ruby");
    nh_set_str_add(set, "s");
    nh_set_str_add(set, "samp");
    nh_set_str_add(set, "script");
    nh_set_str_add(set, "section");
    nh_set_str_add(set, "select");
    nh_set_str_add(set, "shadow");
    nh_set_str_add(set, "shadow");
    nh_set_str_add(set, "slot");
    nh_set_str_add(set, "small");
    nh_set_str_add(set, "source");
    nh_set_str_add(set, "spacer");
    nh_set_str_add(set, "span");
    nh_set_str_add(set, "strike");
    nh_set_str_add(set, "strong");
    nh_set_str_add(set, "style");
    nh_set_str_add(set, "sub");
    nh_set_str_add(set, "summary");
    nh_set_str_add(set, "sup");
    nh_set_str_add(set, "table");
    nh_set_str_add(set, "tbody");
    nh_set_str_add(set, "td");
    nh_set_str_add(set, "template");
    nh_set_str_add(set, "textarea");
    nh_set_str_add(set, "tfoot");
    nh_set_str_add(set, "th");
    nh_set_str_add(set, "thead");
    nh_set_str_add(set, "time");
    nh_set_str_add(set, "title");
    nh_set_str_add(set, "tr");
    nh_set_str_add(set, "track");
    nh_set_str_add(set, "tt");
    nh_set_str_add(set, "tt");
    nh_set_str_add(set, "u");
    nh_set_str_add(set, "ul");
    nh_set_str_add(set, "var");
    nh_set_str_add(set, "video");
    nh_set_str_add(set, "wbr");
    nh_set_str_add(set, "xmp");
}

void hb_rule_tag_html_init(void) {
    hb_rule_tag_html_set = nh_set_str_create();
    hb_rule_tag_html_add_elems(hb_rule_tag_html_set);
}

bool hb_rule_tag_html_check(hb_rune* tag) {
    return nh_set_str_has(hb_rule_tag_html_set, tag);
}
