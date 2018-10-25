#include <stddef.h>
#include <stdlib.h>

#define INITIAL_LIST_SIDE_SIZE 10
#define LIST_GROWTH_RATE 1.5

#define HBU_LIST(name, elem_type, elem_size, invsafe_elem_type, invalid_value)                    \
  typedef struct name##_s                                                                         \
  {                                                                                               \
    elem_type *data;                                                                              \
    size_t head;                                                                                  \
    size_t length;                                                                                \
    size_t size;                                                                                  \
    size_t size_left;                                                                             \
    size_t size_right;                                                                            \
  } * name##_t;                                                                                   \
                                                                                                  \
  name##_t name##_create_size(size_t initial_size_left, size_t initial_size_right)                \
  {                                                                                               \
    size_t initial_size = initial_size_left + initial_size_right;                                 \
    name##_t buf = malloc(sizeof(struct name##_s));                                               \
    buf->data = calloc(initial_size, elem_size);                                                  \
    buf->head = initial_size_left;                                                                \
    buf->length = 0;                                                                              \
    buf->size = initial_size;                                                                     \
    buf->size_left = initial_size_left;                                                           \
    buf->size_right = initial_size_right;                                                         \
    return buf;                                                                                   \
  }                                                                                               \
                                                                                                  \
  name##_t name##_create(void)                                                                    \
  {                                                                                               \
    return name##_create_size(INITIAL_LIST_SIDE_SIZE, INITIAL_LIST_SIDE_SIZE);                    \
  }                                                                                               \
                                                                                                  \
  void name##_destroy(name##_t buf)                                                               \
  {                                                                                               \
    free(buf->data);                                                                              \
    free(buf);                                                                                    \
  }                                                                                               \
                                                                                                  \
  void name##_destroy_shallow(name##_t buf)                                                       \
  {                                                                                               \
    free(buf);                                                                                    \
  }                                                                                               \
                                                                                                  \
  elem_type *name##_underlying(name##_t buf)                                                      \
  {                                                                                               \
    return &((buf->data)[buf->head]);                                                             \
  }                                                                                               \
                                                                                                  \
  elem_type *name##_underlying_copy(name##_t buf)                                                 \
  {                                                                                               \
    elem_type *copy = calloc(buf->length + 1, elem_size);                                         \
    memcpy(copy, name##_underlying(buf), buf->length *elem_size);                                 \
    return copy;                                                                                  \
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
    return buf->data[buf->head + idx];                                                            \
  }                                                                                               \
                                                                                                  \
  void name##_set(name##_t buf, size_t idx, elem_type c)                                          \
  {                                                                                               \
    if (!name##_valid_index(buf, idx))                                                            \
    {                                                                                             \
      return;                                                                                     \
    }                                                                                             \
                                                                                                  \
    buf->data[buf->head + idx] = c;                                                               \
  }                                                                                               \
                                                                                                  \
  void name##_size_expand_left(name##_t buf, size_t new_size_left)                                \
  {                                                                                               \
    size_t old_size_left = buf->size_left;                                                        \
                                                                                                  \
    if (old_size_left >= new_size_left)                                                           \
    {                                                                                             \
      return;                                                                                     \
    }                                                                                             \
                                                                                                  \
    size_t diff_size_left = new_size_left - old_size_left;                                        \
                                                                                                  \
    size_t new_size = new_size_left + buf->size_right;                                            \
                                                                                                  \
    elem_type *new_data = calloc(new_size, elem_size);                                            \
    memcpy(&(new_data[diff_size_left]), buf->data, elem_size * buf->length);                      \
    free(buf->data);                                                                              \
                                                                                                  \
    buf->data = new_data;                                                                         \
    buf->head += old_size_left;                                                                   \
    buf->size = new_size;                                                                         \
    buf->size_left = new_size_left;                                                               \
  }                                                                                               \
                                                                                                  \
  void name##_size_expand_right(name##_t buf, size_t new_size_right)                              \
  {                                                                                               \
    size_t old_size = buf->size;                                                                  \
    size_t old_size_right = buf->size_right;                                                      \
                                                                                                  \
    if (old_size_right >= new_size_right)                                                         \
    {                                                                                             \
      return;                                                                                     \
    }                                                                                             \
                                                                                                  \
    size_t new_size = buf->size_left + new_size_right;                                            \
                                                                                                  \
    elem_type *new_data = realloc(buf->data, elem_size * new_size);                               \
    for (size_t i = old_size; i < new_size; i++)                                                  \
    {                                                                                             \
      new_data[i] = 0;                                                                            \
    }                                                                                             \
                                                                                                  \
    buf->data = new_data;                                                                         \
    buf->size = new_size;                                                                         \
    buf->size_right = new_size_right;                                                             \
  }                                                                                               \
                                                                                                  \
  void name##_ensure_left(name##_t buf, size_t amount)                                            \
  {                                                                                               \
    size_t desired_size = amount + 1;                                                             \
                                                                                                  \
    if (buf->size_left < desired_size)                                                            \
    {                                                                                             \
      name##_size_expand_left(buf, desired_size);                                                 \
    }                                                                                             \
  }                                                                                               \
                                                                                                  \
  void name##_ensure_right(name##_t buf, size_t amount)                                           \
  {                                                                                               \
    size_t desired_size = amount + 1;                                                             \
                                                                                                  \
    if (buf->size_right < desired_size)                                                           \
    {                                                                                             \
      name##_size_expand_right(buf, desired_size);                                                \
    }                                                                                             \
  }                                                                                               \
                                                                                                  \
  void name##_append(name##_t buf, elem_type tail)                                                \
  {                                                                                               \
    size_t next_idx = buf->head + buf->length;                                                    \
                                                                                                  \
    if (next_idx >= buf->size - 1)                                                                \
    {                                                                                             \
      size_t old_size = buf->size_right;                                                          \
      size_t new_size = old_size * LIST_GROWTH_RATE + 1; /* +1 to always guarantee an increase */ \
                                                                                                  \
      name##_size_expand_right(buf, new_size);                                                    \
    }                                                                                             \
                                                                                                  \
    buf->data[next_idx] = tail;                                                                   \
    buf->length++;                                                                                \
  }                                                                                               \
                                                                                                  \
  void name##_extend_arr(name##_t buf, elem_type *ext, size_t ext_len)                            \
  {                                                                                               \
    name##_ensure_right(buf, buf->length + ext_len);                                              \
    for (size_t i = 0; i < ext_len; i++)                                                          \
    {                                                                                             \
      name##_append(buf, ext[i]);                                                                 \
    }                                                                                             \
  }                                                                                               \
                                                                                                  \
  void name##_extend_buf(name##_t buf, name##_t ext)                                              \
  {                                                                                               \
    name##_ensure_right(buf, buf->length + ext->length);                                          \
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
    elem_type f = buf->data[buf->head];                                                           \
    buf->data[buf->head] = 0;                                                                     \
    buf->head++;                                                                                  \
    buf->length--;                                                                                \
                                                                                                  \
    return f;                                                                                     \
  }                                                                                               \
                                                                                                  \
  void name##_unshift(name##_t buf, elem_type head)                                               \
  {                                                                                               \
    if (buf->head == 0)                                                                           \
    {                                                                                             \
      size_t old_size = buf->size_left;                                                           \
      size_t new_size = old_size * LIST_GROWTH_RATE + 1; /* +1 to always guarantee an increase */ \
                                                                                                  \
      name##_size_expand_left(buf, new_size);                                                     \
    }                                                                                             \
                                                                                                  \
    buf->head--;                                                                                  \
    buf->data[buf->head] = head;                                                                  \
    buf->length++;                                                                                \
  }                                                                                               \
                                                                                                  \
  invsafe_elem_type name##_pop(name##_t buf)                                                      \
  {                                                                                               \
    if (buf->length == 0)                                                                         \
    {                                                                                             \
      return invalid_value;                                                                       \
    }                                                                                             \
                                                                                                  \
    size_t idx = buf->head + buf->length - 1;                                                     \
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
