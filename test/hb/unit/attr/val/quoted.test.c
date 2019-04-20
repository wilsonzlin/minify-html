#include <stdio.h>
#include <hb/rule.h>
#include <hb/unit.h>

#define INPUT "'abc&apos;&quot;&quot; a' 1"

int main(void) {
    printf("Test started\n");
    hb_rule_init();

    hb_err_set* suppressed = hb_err_set_create();
    hb_err_set_add(suppressed, HB_ERR_PARSE_MALFORMED_ENTITY);

    hb_rune* out = calloc(sizeof(INPUT) - 1, sizeof(hb_rune));

    hb_cfg cfg = {
            .collapse_whitespace = {
                    .mode = HB_CFG_SET_MODE_ALL,
                    .set = NULL,
            },
            .destroy_whole_whitespace = {
                    .mode = HB_CFG_SET_MODE_ALL,
                    .set = NULL,
            },
            .trim_whitespace = {
                    .mode = HB_CFG_SET_MODE_ALL,
                    .set = NULL,
            },
            .suppressed_errors = *suppressed,
            .trim_class_attributes = true,
            .decode_entities = true,
            .remove_attr_quotes = true,
            .remove_comments = true,
            .remove_optional_tags = true,
            .remove_tag_whitespace = true,
    };

    hb_proc proc = {
        .cfg = &cfg,
        .name = "test",
        .src = INPUT "\xFF",
        .src_len = sizeof(INPUT) - 1,
        .src_next = 0,
        .out = out,
        .out_next = 0,
    };

    hb_unit_attr_type type = hb_unit_attr_val_quoted(&proc, false);
    printf("%s\n", out);

    hb_err_set_destroy(suppressed);
}
