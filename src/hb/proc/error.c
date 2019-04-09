#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <hb/proc.h>

static void hb_proc_error_setandjmp(hb_proc* proc, hb_err code, size_t pos, char* msg, bool custom) {
    proc->result.code = code;
    proc->result.pos = pos;
    proc->result.msg = msg;
    proc->result.custom = custom;
    longjmp(proc->start, 1);
}

void hb_proc_error_pos(hb_proc* proc, hb_err code, size_t pos, char const* msg) {
    hb_proc_error_setandjmp(proc, code, pos, (char*) msg, false);
}

void hb_proc_error_custom_pos(hb_proc* proc, hb_err code, size_t pos, char const* format, ...) {
    va_list args;
    va_start(args, format);

    char* msg = calloc(HB_PROC_ERROR_CUSTOM_SIZE, sizeof(char));
    vsnprintf(msg, HB_PROC_ERROR_CUSTOM_SIZE - 1, format, args);

    va_end(args);

    hb_proc_error_setandjmp(proc, code, pos, msg, true);
}
