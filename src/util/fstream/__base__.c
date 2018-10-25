#include <errno.h>
#include "../execution/error.c"

#define HBU_FSTREAM_BUILD_INFRA(type, mode, noun, verb, std)                                                              \
  typedef struct hbu_fstream##type##_s                                                                                    \
  {                                                                                                                       \
    const char *name;                                                                                                     \
    FILE *fd;                                                                                                             \
  } * hbu_fstream##type##_t;                                                                                              \
                                                                                                                          \
  hbu_fstream##type##_t hbu_fstream##type##_create(hbe_err_t *hbe_err, char *path)                                        \
  {                                                                                                                       \
    hbu_fstream##type##_t fstream = malloc(sizeof(struct hbu_fstream##type##_s));                                         \
                                                                                                                          \
    if (path == NULL)                                                                                                     \
    {                                                                                                                     \
      fstream->name = #std;                                                                                               \
      fstream->fd = std;                                                                                                  \
    }                                                                                                                     \
    else                                                                                                                  \
    {                                                                                                                     \
      fstream->name = path;                                                                                               \
                                                                                                                          \
      FILE *fd = fopen(path, mode);                                                                                       \
                                                                                                                          \
      if (fd == NULL)                                                                                                     \
      {                                                                                                                   \
        HBE_THROW(HBE_IO_FOPEN_FAIL, "Failed to open file %s for " verb " with error %d", fstream->name, errno);          \
      }                                                                                                                   \
                                                                                                                          \
      fstream->fd = fd;                                                                                                   \
    }                                                                                                                     \
                                                                                                                          \
    return fstream;                                                                                                       \
  }                                                                                                                       \
                                                                                                                          \
  void hbu_fstream##type##_destroy(hbe_err_t *hbe_err, hbu_fstream##type##_t fstream)                                     \
  {                                                                                                                       \
    if (fclose(fstream->fd) == EOF)                                                                                       \
    {                                                                                                                     \
      HBE_THROW_V(HBE_IO_FCLOSE_FAIL, "Failed to close " noun " stream for file %s with error %d", fstream->name, errno); \
    }                                                                                                                     \
                                                                                                                          \
    free(fstream);                                                                                                        \
  }
