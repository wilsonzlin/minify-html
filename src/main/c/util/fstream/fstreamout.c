#include <errno.h>
#include "../execution/error.c"
#include "../list/char.c"
#include "./__base__.c"

HBU_FSTREAM_BUILD_INFRA(out, "w", "write", "writing", stdout)

static void _hbu_fstreamout_fwrite(hbe_err_t *hbe_err, hbu_fstreamout_t fstreamout, hb_char_t *source, size_t length) {
  if (fwrite(source, SIZEOF_CHAR, length, fstreamout->fd) != SIZEOF_CHAR * length) {
    HBE_THROW_V(HBE_IO_FWRITE_FAIL, "Failed to write to output file %s", fstreamout->name);
  }
}

void hbu_fstreamout_write_buffer(hbe_err_t *hbe_err, hbu_fstreamout_t fstreamout, hbu_list_char_t buffer) {
  HBE_CATCH_V(_hbu_fstreamout_fwrite, fstreamout, hbu_list_char_underlying(buffer), buffer->length);
}

void hbu_fstreamout_write(hbe_err_t *hbe_err, hbu_fstreamout_t fstreamout, hb_char_t c) {
  HBE_CATCH_V(_hbu_fstreamout_fwrite, fstreamout, &c, 1);
}
