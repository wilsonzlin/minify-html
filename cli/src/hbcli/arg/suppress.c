#include <stddef.h>
#include <string.h>
#include <hb/err.h>
#include <hbcli/arg.h>
#include <hbcli/err.h>

void hbcli_arg_suppress_parse(hb_err_set* suppressed_errors, char *argv) {
    if (argv == NULL) {
        return;
    }

    if (strcmp(argv, "MALFORMED_ENTITY") == 0) {
        hb_err_set_add(suppressed_errors, HB_ERR_PARSE_MALFORMED_ENTITY);

    } else if (strcmp(argv, "INVALID_ENTITY") == 0) {
        hb_err_set_add(suppressed_errors, HB_ERR_PARSE_INVALID_ENTITY);

    } else if (strcmp(argv, "NONSTANDARD_TAG") == 0) {
        hb_err_set_add(suppressed_errors, HB_ERR_PARSE_NONSTANDARD_TAG);

    } else if (strcmp(argv, "UCASE_ATTR") == 0) {
        hb_err_set_add(suppressed_errors, HB_ERR_PARSE_UCASE_ATTR);

    } else if (strcmp(argv, "UCASE_TAG") == 0) {
        hb_err_set_add(suppressed_errors, HB_ERR_PARSE_UCASE_TAG);

    } else if (strcmp(argv, "UNQUOTED_ATTR") == 0) {
        hb_err_set_add(suppressed_errors, HB_ERR_PARSE_UNQUOTED_ATTR);

    } else if (strcmp(argv, "SELF_CLOSING_TAG") == 0) {
        hb_err_set_add(suppressed_errors, HB_ERR_PARSE_SELF_CLOSING_TAG);

    } else {
        hbcli_err("Unrecognised suppressable error `%s`", argv);
    }
}
