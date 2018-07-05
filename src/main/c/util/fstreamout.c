#ifndef _HDR_HYPERBUILD_UTIL_FSTREAMOUT
#define _HDR_HYPERBUILD_UTIL_FSTREAMOUT

#include <errno.h>
#include <stdio.h>
#include "../error/error.c"
#include "fstream.h"

HBU_FSTREAM_BUILD_INFRA(out, "w", "write", "writing")

void hbu_fstreamout_write(hbu_fstreamout_t fstreamout, hb_char_t c) {
  if (fwrite(&c, SIZEOF_CHAR, 1, fstreamout->fd) != SIZEOF_CHAR) {
    hbe_fatal(HBE_IO_FWRITE_FAIL, "Failed to write to output file %s", fstreamout->name);
  }
}

#endif // _HDR_HYPERBUILD_UTIL_FSTREAMOUT
