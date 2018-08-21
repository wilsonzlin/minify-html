#include <errno.h>
#include "../char/char.c"
#include "../execution/error.c"
#include "./__base__.c"

HBU_FSTREAM_BUILD_INFRA(in, "r", "read", "reading", stdin)

hb_eod_char_t hbu_fstreamin_read(hbe_err_t *hbe_err, hbu_fstreamin_t fstreamin) {
  hb_char_t c;

  if (fread(&c, SIZEOF_CHAR, 1, fstreamin->fd) != SIZEOF_CHAR) {
    if (ferror(fstreamin->fd)) {
      HBE_THROW(HBE_IO_FREAD_FAIL, "Failed to read input file %s", fstreamin->name);
    }

    // Must be EOF
    return HB_EOD;
  }

  return c;
}
