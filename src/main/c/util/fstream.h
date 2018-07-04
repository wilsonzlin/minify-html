#ifndef _HDR_HYPERBUILD_UTIL_FSTREAM
#define _HDR_HYPERBUILD_UTIL_FSTREAM

#include <errno.h>
#include <stdio.h>
#include "../error/error.c"

#define HBU_FSTREAM_BUILD_INFRA(type, mode, noun, verb) \
  typedef struct hbu_fstream##type##_s { \
    const char *name; \
    FILE *fd; \
  } *hbu_fstream##type##_t; \
  \
  hbu_fstream##type##_t hbu_fstream##type##_create(char *path) { \
    hbu_fstream##type##_t fstream = malloc(sizeof(struct hbu_fstream##type##_s)); \
    fstream->name = path; \
    \
    FILE *fd = fopen(path, mode); \
    \
    if (fd == NULL) { \
      hbe_fatal("Failed to open file %s for " verb, fstream->name); \
    } \
    \
    fstream->fd = fd; \
    \
    return fstream; \
  } \
  \
  void hbu_fstream##type##_delete(hbu_fstream##type##_t fstream) { \
    if (fclose(fstream->fd) == EOF) { \
      hbe_fatal("Failed to close " noun " stream for file %s", fstream->name); \
    } \
    \
    free(fstream); \
  }

#endif // _HDR_HYPERBUILD_UTIL_FSTREAM
