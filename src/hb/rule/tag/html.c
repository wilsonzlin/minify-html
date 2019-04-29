#include <hb/collection.h>
#include <hb/rule.h>
#include <hb/rune.h>

// Sourced from https://developer.mozilla.org/en-US/docs/Web/HTML/Element at
// 2018-07-01T05:55:00Z

static hb_set_tag_names* hb_rule_tag_html_set;

void hb_rule_tag_html_add_elems(hb_set_tag_names* set)
{
	hb_set_tag_names_add_whole_literal(set, "a");
	hb_set_tag_names_add_whole_literal(set, "abbr");
	hb_set_tag_names_add_whole_literal(set, "acronym");
	hb_set_tag_names_add_whole_literal(set, "address");
	hb_set_tag_names_add_whole_literal(set, "applet");
	hb_set_tag_names_add_whole_literal(set, "applet");
	hb_set_tag_names_add_whole_literal(set, "area");
	hb_set_tag_names_add_whole_literal(set, "article");
	hb_set_tag_names_add_whole_literal(set, "aside");
	hb_set_tag_names_add_whole_literal(set, "audio");
	hb_set_tag_names_add_whole_literal(set, "b");
	hb_set_tag_names_add_whole_literal(set, "basefont");
	hb_set_tag_names_add_whole_literal(set, "bdi");
	hb_set_tag_names_add_whole_literal(set, "bdo");
	hb_set_tag_names_add_whole_literal(set, "bgsound");
	hb_set_tag_names_add_whole_literal(set, "big");
	hb_set_tag_names_add_whole_literal(set, "blink");
	hb_set_tag_names_add_whole_literal(set, "blockquote");
	hb_set_tag_names_add_whole_literal(set, "body");
	hb_set_tag_names_add_whole_literal(set, "br");
	hb_set_tag_names_add_whole_literal(set, "button");
	hb_set_tag_names_add_whole_literal(set, "canvas");
	hb_set_tag_names_add_whole_literal(set, "caption");
	hb_set_tag_names_add_whole_literal(set, "center");
	hb_set_tag_names_add_whole_literal(set, "cite");
	hb_set_tag_names_add_whole_literal(set, "code");
	hb_set_tag_names_add_whole_literal(set, "col");
	hb_set_tag_names_add_whole_literal(set, "colgroup");
	hb_set_tag_names_add_whole_literal(set, "command");
	hb_set_tag_names_add_whole_literal(set, "content");
	hb_set_tag_names_add_whole_literal(set, "content");
	hb_set_tag_names_add_whole_literal(set, "data");
	hb_set_tag_names_add_whole_literal(set, "datalist");
	hb_set_tag_names_add_whole_literal(set, "dd");
	hb_set_tag_names_add_whole_literal(set, "del");
	hb_set_tag_names_add_whole_literal(set, "details");
	hb_set_tag_names_add_whole_literal(set, "dfn");
	hb_set_tag_names_add_whole_literal(set, "dialog");
	hb_set_tag_names_add_whole_literal(set, "dir");
	hb_set_tag_names_add_whole_literal(set, "dir");
	hb_set_tag_names_add_whole_literal(set, "div");
	hb_set_tag_names_add_whole_literal(set, "dl");
	hb_set_tag_names_add_whole_literal(set, "dt");
	hb_set_tag_names_add_whole_literal(set, "element");
	hb_set_tag_names_add_whole_literal(set, "element");
	hb_set_tag_names_add_whole_literal(set, "em");
	hb_set_tag_names_add_whole_literal(set, "embed");
	hb_set_tag_names_add_whole_literal(set, "fieldset");
	hb_set_tag_names_add_whole_literal(set, "figcaption");
	hb_set_tag_names_add_whole_literal(set, "figure");
	hb_set_tag_names_add_whole_literal(set, "font");
	hb_set_tag_names_add_whole_literal(set, "footer");
	hb_set_tag_names_add_whole_literal(set, "form");
	hb_set_tag_names_add_whole_literal(set, "frame");
	hb_set_tag_names_add_whole_literal(set, "frameset");
	hb_set_tag_names_add_whole_literal(set, "h1");
	hb_set_tag_names_add_whole_literal(set, "h2");
	hb_set_tag_names_add_whole_literal(set, "h3");
	hb_set_tag_names_add_whole_literal(set, "h4");
	hb_set_tag_names_add_whole_literal(set, "h5");
	hb_set_tag_names_add_whole_literal(set, "h6");
	hb_set_tag_names_add_whole_literal(set, "head");
	hb_set_tag_names_add_whole_literal(set, "header");
	hb_set_tag_names_add_whole_literal(set, "hgroup");
	hb_set_tag_names_add_whole_literal(set, "hr");
	hb_set_tag_names_add_whole_literal(set, "html");
	hb_set_tag_names_add_whole_literal(set, "i");
	hb_set_tag_names_add_whole_literal(set, "iframe");
	hb_set_tag_names_add_whole_literal(set, "image");
	hb_set_tag_names_add_whole_literal(set, "img");
	hb_set_tag_names_add_whole_literal(set, "input");
	hb_set_tag_names_add_whole_literal(set, "ins");
	hb_set_tag_names_add_whole_literal(set, "isindex");
	hb_set_tag_names_add_whole_literal(set, "kbd");
	hb_set_tag_names_add_whole_literal(set, "keygen");
	hb_set_tag_names_add_whole_literal(set, "label");
	hb_set_tag_names_add_whole_literal(set, "legend");
	hb_set_tag_names_add_whole_literal(set, "li");
	hb_set_tag_names_add_whole_literal(set, "link");
	hb_set_tag_names_add_whole_literal(set, "listing");
	hb_set_tag_names_add_whole_literal(set, "main");
	hb_set_tag_names_add_whole_literal(set, "map");
	hb_set_tag_names_add_whole_literal(set, "mark");
	hb_set_tag_names_add_whole_literal(set, "marquee");
	hb_set_tag_names_add_whole_literal(set, "menu");
	hb_set_tag_names_add_whole_literal(set, "menuitem");
	hb_set_tag_names_add_whole_literal(set, "menuitem");
	hb_set_tag_names_add_whole_literal(set, "meta");
	hb_set_tag_names_add_whole_literal(set, "meter");
	hb_set_tag_names_add_whole_literal(set, "multicol");
	hb_set_tag_names_add_whole_literal(set, "nav");
	hb_set_tag_names_add_whole_literal(set, "nextid");
	hb_set_tag_names_add_whole_literal(set, "nobr");
	hb_set_tag_names_add_whole_literal(set, "noembed");
	hb_set_tag_names_add_whole_literal(set, "noembed");
	hb_set_tag_names_add_whole_literal(set, "noframes");
	hb_set_tag_names_add_whole_literal(set, "noscript");
	hb_set_tag_names_add_whole_literal(set, "object");
	hb_set_tag_names_add_whole_literal(set, "ol");
	hb_set_tag_names_add_whole_literal(set, "optgroup");
	hb_set_tag_names_add_whole_literal(set, "option");
	hb_set_tag_names_add_whole_literal(set, "output");
	hb_set_tag_names_add_whole_literal(set, "p");
	hb_set_tag_names_add_whole_literal(set, "param");
	hb_set_tag_names_add_whole_literal(set, "picture");
	hb_set_tag_names_add_whole_literal(set, "plaintext");
	hb_set_tag_names_add_whole_literal(set, "pre");
	hb_set_tag_names_add_whole_literal(set, "progress");
	hb_set_tag_names_add_whole_literal(set, "q");
	hb_set_tag_names_add_whole_literal(set, "rp");
	hb_set_tag_names_add_whole_literal(set, "rt");
	hb_set_tag_names_add_whole_literal(set, "rtc");
	hb_set_tag_names_add_whole_literal(set, "ruby");
	hb_set_tag_names_add_whole_literal(set, "s");
	hb_set_tag_names_add_whole_literal(set, "samp");
	hb_set_tag_names_add_whole_literal(set, "script");
	hb_set_tag_names_add_whole_literal(set, "section");
	hb_set_tag_names_add_whole_literal(set, "select");
	hb_set_tag_names_add_whole_literal(set, "shadow");
	hb_set_tag_names_add_whole_literal(set, "shadow");
	hb_set_tag_names_add_whole_literal(set, "slot");
	hb_set_tag_names_add_whole_literal(set, "small");
	hb_set_tag_names_add_whole_literal(set, "source");
	hb_set_tag_names_add_whole_literal(set, "spacer");
	hb_set_tag_names_add_whole_literal(set, "span");
	hb_set_tag_names_add_whole_literal(set, "strike");
	hb_set_tag_names_add_whole_literal(set, "strong");
	hb_set_tag_names_add_whole_literal(set, "style");
	hb_set_tag_names_add_whole_literal(set, "sub");
	hb_set_tag_names_add_whole_literal(set, "summary");
	hb_set_tag_names_add_whole_literal(set, "sup");
	hb_set_tag_names_add_whole_literal(set, "table");
	hb_set_tag_names_add_whole_literal(set, "tbody");
	hb_set_tag_names_add_whole_literal(set, "td");
	hb_set_tag_names_add_whole_literal(set, "template");
	hb_set_tag_names_add_whole_literal(set, "textarea");
	hb_set_tag_names_add_whole_literal(set, "tfoot");
	hb_set_tag_names_add_whole_literal(set, "th");
	hb_set_tag_names_add_whole_literal(set, "thead");
	hb_set_tag_names_add_whole_literal(set, "time");
	hb_set_tag_names_add_whole_literal(set, "title");
	hb_set_tag_names_add_whole_literal(set, "tr");
	hb_set_tag_names_add_whole_literal(set, "track");
	hb_set_tag_names_add_whole_literal(set, "tt");
	hb_set_tag_names_add_whole_literal(set, "tt");
	hb_set_tag_names_add_whole_literal(set, "u");
	hb_set_tag_names_add_whole_literal(set, "ul");
	hb_set_tag_names_add_whole_literal(set, "var");
	hb_set_tag_names_add_whole_literal(set, "video");
	hb_set_tag_names_add_whole_literal(set, "wbr");
	hb_set_tag_names_add_whole_literal(set, "xmp");
}

void hb_rule_tag_html_init(void)
{
	hb_rule_tag_html_set = hb_set_tag_names_create();
	hb_rule_tag_html_add_elems(hb_rule_tag_html_set);
}

bool hb_rule_tag_html_check(nh_view_str* tag)
{
	return hb_set_tag_names_has(hb_rule_tag_html_set, tag);
}
