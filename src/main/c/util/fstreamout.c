#ifndef _HDR_HYPERBUILD_UTIL_FSTREAMOUT
#define _HDR_HYPERBUILD_UTIL_FSTREAMOUT

#include <errno.h>
#include <stdio.h>
#include "../error/error.c"
#include "./buffer.c"
#include "./fstream.h"

HBU_FSTREAM_BUILD_INFRA(out, "w", "write", "writing", stdout)

static void _hbu_fstreamout_fwrite(hbu_fstreamout_t fstreamout, hb_char_t *source, size_t length) {
  if (fwrite(source, SIZEOF_CHAR, length, fstreamout->fd) != SIZEOF_CHAR * length) {
    hbe_fatal(HBE_IO_FWRITE_FAIL, "Failed to write to output file %s", fstreamout->name);
  }
}

void hbu_fstreamout_write_buffer(hbu_fstreamout_t fstreamout, hbu_buffer_t buffer) {
  _hbu_fstreamout_fwrite(fstreamout, hbu_buffer_underlying(buffer), buffer->length);
}

void hbu_fstreamout_write(hbu_fstreamout_t fstreamout, hb_char_t c) {
  _hbu_fstreamout_fwrite(fstreamout, &c, 1);
}

#endif // _HDR_HYPERBUILD_UTIL_FSTREAMOUT
