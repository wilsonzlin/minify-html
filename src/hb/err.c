#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <hb/err.h>

hb_err_set* hb_err_set_create(void) {
    hb_err_set* set = calloc(1, sizeof(hb_err_set));
    return set;
}

void hb_err_set_add(hb_err_set* set, hb_err err) {
    set->set[err] = true;
}

void hb_err_set_remove(hb_err_set* set, hb_err err) {
    set->set[err] = false;
}

bool hb_err_set_has(hb_err_set* set, hb_err err) {
    return set->set[err];
}
