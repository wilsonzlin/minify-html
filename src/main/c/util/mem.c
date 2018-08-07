#ifndef _HDR_HYPERBUILD_UTIL_MEM
#define _HDR_HYPERBUILD_UTIL_MEM

#include "../error/error.c"

void *hbu_mem_malloc(size_t size) {
  void *ptr = malloc(size);
  if (ptr == NULL) {
    hbe_fatal(HBE_MEM_ALLOC_FAIL, "Failed to allocate memory; possibly out of memory");
  }
  return ptr;
}

void *hbu_mem_calloc(size_t nmemb, size_t size) {
  void *ptr = calloc(nmemb, size);
  if (ptr == NULL) {
    hbe_fatal(HBE_MEM_ALLOC_FAIL, "Failed to allocate memory; possibly out of memory");
  }
  return ptr;
}

void *hbu_mem_realloc(void *ptr, size_t size) {
  void *reptr = realloc(ptr, size);
  if (reptr == NULL) {
    hbe_fatal(HBE_MEM_ALLOC_FAIL, "Failed to allocate memory; possibly out of memory");
  }
  return reptr;
}

#endif // _HDR_HYPERBUILD_UTIL_MEM
