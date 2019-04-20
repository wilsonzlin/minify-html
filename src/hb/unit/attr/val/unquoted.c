#include <stdbool.h>
#include <hb/unit.h>
#include <hb/proc.h>
#include <hb/rule.h>

void hb_unit_attr_val_unquoted(hb_proc* proc) {
    bool at_least_one_char = false;

    hb_rune c;
    while (true) {
        c = hb_proc_peek(proc);
        if (!hb_rule_attr_unquotedvalue_check(c)) {
            break;
        }
        at_least_one_char = true;

        if (c == '&') {
            // Process entity.
            hb_unit_entity(proc);
        } else {
            hb_proc_accept(proc);
        }
    }

    if (!at_least_one_char) {
        hb_proc_error_custom(proc, HB_ERR_PARSE_EXPECTED_NOT_FOUND, "Expected unquoted attribute value, got `%c` (U+%x)", c);
    }
}
