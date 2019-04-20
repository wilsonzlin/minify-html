#include <stdbool.h>
#include <hb/collection.h>
#include <hb/rule.h>
#include <hb/proc.h>
#include <hb/unit.h>

hb_unit_attr_type hb_unit_attr(hb_proc* proc) {
    hb_proc_view_init_src(name, proc);

    hb_proc_view_start_with_src_next(&name, proc);
    do {
        // Require at least one character.
        hb_rune c = hb_proc_require_predicate(proc, &hb_rule_attr_name_check, "attribute name");

        if (hb_rule_ascii_uppercase_check(c)) {
            hb_proc_error_if_not_suppressed(proc, HB_ERR_PARSE_UCASE_ATTR, "Uppercase letter in attribute name");
        }
    } while (hb_rule_attr_name_check(hb_proc_peek(proc)));
    hb_proc_view_end_with_src_prev(&name, proc);

    bool should_collapse_and_trim_value_ws = nh_view_str_equals_literal_i(&name, "class") && proc->cfg->trim_class_attributes;
    bool has_value = hb_proc_accept_if(proc, '=');
    hb_unit_attr_type attr_type = HB_UNIT_ATTR_NOVAL;

    if (has_value) {
        hb_rune next = hb_proc_peek(proc);

        if (hb_rule_attr_quote_check(next)) {
            // Quoted attribute value.
            attr_type = hb_unit_attr_val_quoted(proc, should_collapse_and_trim_value_ws);
        } else {
            // Unquoted attribute value.
            hb_proc_error_if_not_suppressed(proc, HB_ERR_PARSE_UNQUOTED_ATTR, "Unquoted attribute value");
            attr_type = HB_UNIT_ATTR_UNQUOTED;
            hb_unit_attr_val_unquoted(proc);
        }
    }

    return attr_type;
}
