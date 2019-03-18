#pragma once

#include <hb/rune.h>
#include <hb/proc.h>

/**
 * Gets the next character.
 * If it's the end, {@link HB_EOF} is returned.
 *
 * @param proc proc
 * @return character or {@link HB_EOF}
 * @throws on read error
 */
hb_rune hb_proc_peek_eof(hb_proc* proc) {
    if (proc->eof) {
        return HB_EOF;
    }

    return proc->src[proc->src_next];
}
