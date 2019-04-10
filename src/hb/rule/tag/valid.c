#pragma once

#include <hb/rune.h>
#include <hb/rule.h>

bool hb_rule_tag_valid_check(hb_rune* tag) {
    return hb_rule_tag_html_check(tag) ||
        hb_rule_tag_svg_check(tag);
}
