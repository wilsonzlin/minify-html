#include <hb/collection.h>
#include <hb/proc.h>
#include <hb/rule.h>

nh_view_str hb_unit_tag_name(hb_proc* proc) {
    hb_proc_view_init_src(name, proc);

    hb_proc_view_start_with_src_next(&name, proc);
    do {
        // Require at least one character.
        hb_rune c = hb_proc_require_predicate(proc, &hb_rule_tag_name_check, "tag name");

        if (hb_rule_ascii_uppercase_check(c)) {
            hb_proc_error_if_not_suppressed(proc, HB_ERR_PARSE_UCASE_TAG, "Uppercase letter in tag name");
        }
    } while (hb_rule_tag_name_check(hb_proc_peek(proc)));
    hb_proc_view_end_with_src_prev(&name, proc);

    if (!hb_rule_tag_valid_check(&name)) {
        hb_proc_error_if_not_suppressed(proc, HB_ERR_PARSE_NONSTANDARD_TAG, "Non-standard tag");
    }

    return name;
}
