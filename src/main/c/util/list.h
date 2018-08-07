#ifndef _HDR_HYPERBUILD_UTIL_LIST
#define _HDR_HYPERBUILD_UTIL_LIST

#include <stddef.h>
#include <stdlib.h>

#define INITIAL_LIST_SIZE 20
#define LIST_GROWTH_RATE 1.5

#define HBU_LIST(name, elem_type, elem_size, invsafe_elem_type, invalid_value)                    \
  typedef struct name##_s                                                                         \
  {                                                                                               \
    elem_type *data;                                                                              \
    size_t length;                                                                                \
    size_t size;                                                                                  \
  } * name##_t;                                                                                   \
                                                                                                  \
  name##_t name##_create_size(size_t initial_list_size)                                           \
  {                                                                                               \
    name##_t buf = hbu_mem_malloc(sizeof(struct name##_s));                                       \
    buf->data = hbu_mem_calloc(initial_list_size, elem_size);                                     \
    buf->length = 0;                                                                              \
    buf->size = initial_list_size;                                                                \
    return buf;                                                                                   \
  }                                                                                               \
                                                                                                  \
  name##_t name##_create(void)                                                                    \
  {                                                                                               \
    return name##_create_size(INITIAL_LIST_SIZE);                                                 \
  }                                                                                               \
                                                                                                  \
  void name##_destroy(name##_t buf)                                                               \
  {                                                                                               \
    free(buf->data);                                                                              \
    free(buf);                                                                                    \
  }                                                                                               \
                                                                                                  \
  elem_type *name##_underlying(name##_t buf)                                                      \
  {                                                                                               \
    return buf->data;                                                                             \
  }                                                                                               \
                                                                                                  \
  int name##_valid_index(name##_t buf, size_t idx)                                                \
  {                                                                                               \
    return idx < buf->length;                                                                     \
  }                                                                                               \
                                                                                                  \
  invsafe_elem_type name##_get(name##_t buf, size_t idx)                                          \
  {                                                                                               \
    if (!name##_valid_index(buf, idx))                                                            \
    {                                                                                             \
      return invalid_value;                                                                       \
    }                                                                                             \
                                                                                                  \
    return buf->data[idx];                                                                        \
  }                                                                                               \
                                                                                                  \
  void name##_set(name##_t buf, size_t idx, elem_type c)                                          \
  {                                                                                               \
    if (!name##_valid_index(buf, idx))                                                            \
    {                                                                                             \
      return;                                                                                     \
    }                                                                                             \
                                                                                                  \
    buf->data[idx] = c;                                                                           \
  }                                                                                               \
                                                                                                  \
  void name##_size_expand(name##_t buf, size_t new_size)                                          \
  {                                                                                               \
    size_t old_size = buf->size;                                                                  \
                                                                                                  \
    if (old_size >= new_size)                                                                     \
    {                                                                                             \
      return;                                                                                     \
    }                                                                                             \
                                                                                                  \
    elem_type *new_data = hbu_mem_realloc(buf->data, SIZEOF_CHAR * new_size);                     \
    for (size_t i = old_size; i < new_size; i++)                                                  \
    {                                                                                             \
      new_data[i] = 0;                                                                            \
    }                                                                                             \
                                                                                                  \
    free(buf->data);                                                                              \
                                                                                                  \
    buf->data = new_data;                                                                         \
    buf->size = new_size;                                                                         \
  }                                                                                               \
                                                                                                  \
  void name##_size_shrink(name##_t buf, size_t new_size)                                          \
  {                                                                                               \
    size_t old_size = buf->size;                                                                  \
                                                                                                  \
    if (old_size <= new_size)                                                                     \
    {                                                                                             \
      return;                                                                                     \
    }                                                                                             \
                                                                                                  \
    elem_type *new_data = hbu_mem_malloc(SIZEOF_CHAR * new_size);                                 \
    memcpy(new_data, buf->data, SIZEOF_CHAR *(new_size)-1);                                       \
    new_data[new_size - 1] = 0;                                                                   \
                                                                                                  \
    free(buf->data);                                                                              \
                                                                                                  \
    buf->data = new_data;                                                                         \
    buf->size = new_size;                                                                         \
  }                                                                                               \
                                                                                                  \
  void name##_size_adjust(name##_t buf, size_t new_size)                                          \
  {                                                                                               \
    size_t old_size = buf->size;                                                                  \
                                                                                                  \
    if (new_size > old_size)                                                                      \
    {                                                                                             \
      name##_size_expand(buf, new_size);                                                          \
    }                                                                                             \
    else if (new_size < old_size)                                                                 \
    {                                                                                             \
      name##_size_shrink(buf, new_size);                                                          \
    }                                                                                             \
  }                                                                                               \
                                                                                                  \
  void name##_size_collapse(name##_t buf)                                                         \
  {                                                                                               \
    size_t length = buf->length;                                                                  \
    size_t desired_size = length + 1;                                                             \
                                                                                                  \
    if (buf->size > desired_size)                                                                 \
    {                                                                                             \
      name##_size_shrink(buf, desired_size);                                                      \
    }                                                                                             \
  }                                                                                               \
                                                                                                  \
  void name##_ensure_length(name##_t buf, size_t length)                                          \
  {                                                                                               \
    size_t desired_size = length + 1;                                                             \
                                                                                                  \
    if (buf->size < desired_size)                                                                 \
    {                                                                                             \
      name##_size_expand(buf, desired_size);                                                      \
    }                                                                                             \
  }                                                                                               \
                                                                                                  \
  void name##_append(name##_t buf, elem_type tail)                                                \
  {                                                                                               \
    size_t next_idx = buf->length;                                                                \
                                                                                                  \
    if (next_idx >= buf->size - 1)                                                                \
    {                                                                                             \
      size_t old_size = buf->size;                                                                \
      size_t new_size = old_size * LIST_GROWTH_RATE + 1; /* +1 to always guarantee an increase */ \
                                                                                                  \
      name##_size_expand(buf, new_size);                                                          \
    }                                                                                             \
                                                                                                  \
    buf->data[next_idx] = tail;                                                                   \
    buf->length++;                                                                                \
  }                                                                                               \
                                                                                                  \
  void name##_extend_str(name##_t buf, elem_type *ext, size_t ext_len)                            \
  {                                                                                               \
    name##_ensure_length(buf, buf->length + ext_len);                                             \
    for (size_t i = 0; i < ext_len; i++)                                                          \
    {                                                                                             \
      name##_append(buf, ext[i]);                                                                 \
    }                                                                                             \
  }                                                                                               \
                                                                                                  \
  void name##_extend_buf(name##_t buf, name##_t ext)                                              \
  {                                                                                               \
    name##_ensure_length(buf, buf->length + ext->length);                                         \
    for (size_t i = 0; i < buf->length; i++)                                                      \
    {                                                                                             \
      name##_append(buf, buf->data[i]);                                                           \
    }                                                                                             \
  }                                                                                               \
                                                                                                  \
  invsafe_elem_type name##_shift(name##_t buf)                                                    \
  {                                                                                               \
    if (buf->length == 0)                                                                         \
    {                                                                                             \
      return invalid_value;                                                                       \
    }                                                                                             \
                                                                                                  \
    elem_type f = buf->data[0];                                                                   \
                                                                                                  \
    for (size_t i = 1; i < buf->length; i++)                                                      \
    {                                                                                             \
      buf->data[i - 1] = buf->data[i];                                                            \
    }                                                                                             \
                                                                                                  \
    buf->data[buf->length] = 0;                                                                   \
    buf->length--;                                                                                \
                                                                                                  \
    return f;                                                                                     \
  }                                                                                               \
                                                                                                  \
  void name##_unshift(name##_t buf, elem_type head)                                               \
  {                                                                                               \
    name##_append(buf, 0); /* Use append instead of ensure_length to grow rather than crawl */    \
                                                                                                  \
    for (size_t i = 0; i < buf->length - 1; i++)                                                  \
    { /* -1 to skip last appended 0 */                                                            \
      buf->data[i + 1] = buf->data[i];                                                            \
    }                                                                                             \
                                                                                                  \
    buf->data[0] = head;                                                                          \
    /* length already adjusted thanks to initial append */                                        \
  }                                                                                               \
                                                                                                  \
  invsafe_elem_type name##_pop(name##_t buf)                                                      \
  {                                                                                               \
    if (buf->length == 0)                                                                         \
    {                                                                                             \
      return invalid_value;                                                                       \
    }                                                                                             \
                                                                                                  \
    size_t idx = buf->length - 1;                                                                 \
                                                                                                  \
    elem_type l = buf->data[idx];                                                                 \
                                                                                                  \
    buf->data[idx] = 0;                                                                           \
    buf->length--;                                                                                \
                                                                                                  \
    return l;                                                                                     \
  }                                                                                               \
                                                                                                  \
  int name##_compare(name##_t a, name##_t b);                                                     \
                                                                                                  \
  int name##_equal(name##_t a, name##_t b)                                                        \
  {                                                                                               \
    return a->length == b->length &&                                                              \
           name##_compare(a, b) == 0;                                                             \
  }

#endif
