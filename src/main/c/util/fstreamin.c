#ifndef _HDR_HYPERBUILD_UTIL_FSTREAMIN
#define _HDR_HYPERBUILD_UTIL_FSTREAMIN

#include <errno.h>
#include <stdio.h>
#include "hbchar.h"
#include "../error/error.c"
#include "fstream.h"

HBU_FSTREAM_BUILD_INFRA(in, "r", "read", "reading", stdin)

hb_eod_char_t hbu_fstreamin_read(hbu_fstreamin_t fstreamin) {
  hb_char_t c;

  if (fread(&c, SIZEOF_CHAR, 1, fstreamin->fd) != SIZEOF_CHAR) {
    if (ferror(fstreamin->fd)) {
      hbe_fatal(HBE_IO_FREAD_FAIL, "Failed to read input file %s", fstreamin->name);
    }

    // Must be EOF
    return HB_EOD;
  }

  return c;
}

#endif // _HDR_HYPERBUILD_UTIL_FSTREAMIN
