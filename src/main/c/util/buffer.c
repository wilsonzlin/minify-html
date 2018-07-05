#ifndef _HDR_HYPERBUILD_UTIL_BUFFER
#define _HDR_HYPERBUILD_UTIL_BUFFER

#include <stddef.h>
#include <string.h>
#include "hbchar.h"

#define INITIAL_BUFFER_SIZE 20
#define BUFFER_GROWTH_RATE 1.5

typedef struct hbu_buffer_s {
  hb_char_t *data;
  size_t length;
  size_t size;
} *hbu_buffer_t;

hbu_buffer_t hbu_buffer_create(void) {
  hbu_buffer_t buf = malloc(sizeof(struct hbu_buffer_s));
  buf->data = calloc(INITIAL_BUFFER_SIZE, SIZEOF_CHAR);
  buf->length = 0;
  buf->size = INITIAL_BUFFER_SIZE;
  return buf;
}

void hbu_buffer_destroy(hbu_buffer_t buf) {
  free(buf->data);
  free(buf);
}

hb_char_t *hbu_buffer_underlying(hbu_buffer_t buf) {
  return buf->data;
}

int hbu_buffer_valid_index(hbu_buffer_t buf, size_t idx) {
  return idx < buf->length;
}

hb_eod_char_t hbu_buffer_get(hbu_buffer_t buf, size_t idx) {
  if (!hbu_buffer_valid_index(buf, idx)) {
    return HB_EOD;
  }

  return buf->data[idx];
}

void hbu_buffer_set(hbu_buffer_t buf, size_t idx, hb_char_t c) {
  if (!hbu_buffer_valid_index(buf, idx)) {
    return;
  }

  buf->data[idx] = c;
}

void hbu_buffer_size_expand(hbu_buffer_t buf, size_t new_size) {
  size_t old_size = buf->size;

  if (old_size >= new_size) {
    return;
  }

  hb_char_t *new_data = realloc(buf->data, SIZEOF_CHAR * new_size);
  for (size_t i = old_size; i < new_size; i++) {
    new_data[i] = '\0';
  }

  free(buf->data);

  buf->data = new_data;
  buf->size = new_size;
}

void hbu_buffer_size_shrink(hbu_buffer_t buf, size_t new_size) {
  size_t old_size = buf->size;

  if (old_size <= new_size) {
    return;
  }

  hb_char_t *new_data = malloc(SIZEOF_CHAR * new_size);
  memcpy(new_data, buf->data, SIZEOF_CHAR * (new_size) - 1);
  new_data[new_size - 1] = '\0';

  free(buf->data);

  buf->data = new_data;
  buf->size = new_size;
}

void hbu_buffer_size_adjust(hbu_buffer_t buf, size_t new_size) {
  size_t old_size = buf->size;

  if (new_size > old_size) {
    hbu_buffer_size_expand(buf, new_size);

  } else if (new_size < old_size) {
    hbu_buffer_size_shrink(buf, new_size);
  }
}

void hbu_buffer_size_collapse(hbu_buffer_t buf) {
  size_t length = buf->length;
  size_t desired_size = length + 1;

  if (buf->size > desired_size) {
    hbu_buffer_size_shrink(buf, desired_size);
  }
}

void hbu_buffer_ensure_length(hbu_buffer_t buf, size_t length) {
  size_t desired_size = length + 1;

  if (buf->size < desired_size) {
    hbu_buffer_size_expand(buf, desired_size);
  }
}

void hbu_buffer_append(hbu_buffer_t buf, hb_char_t tail) {
  size_t next_idx = buf->length;

  if (next_idx >= buf->size - 1) {
    size_t old_size = buf->size;
    size_t new_size = old_size * BUFFER_GROWTH_RATE + 1; // +1 to always guarantee an increase

    hbu_buffer_size_expand(buf, new_size);
  }

  buf->data[next_idx] = tail;
  buf->length++;
}

void hbu_buffer_extend_str(hbu_buffer_t buf, hb_char_t *ext, size_t ext_len) {
  hbu_buffer_ensure_length(buf, buf->length + ext_len);
  for (size_t i = 0; i < ext_len; i++) {
    hbu_buffer_append(buf, ext[i]);
  }
}

void hbu_buffer_extend_buf(hbu_buffer_t buf, hbu_buffer_t ext) {
  hbu_buffer_ensure_length(buf, buf->length + ext->length);
  for (size_t i = 0; i < buf->length; i++) {
    hbu_buffer_append(buf, buf->data[i]);
  }
}

hb_eod_char_t hbu_buffer_shift(hbu_buffer_t buf) {
  if (buf->length == 0) {
    return HB_EOD;
  }

  hb_char_t f = buf->data[0];

  for (size_t i = 1; i < buf->length; i++) {
    buf->data[i - 1] = buf->data[i];
  }

  buf->data[buf->length] = '\0';
  buf->length--;

  return f;
}

void hbu_buffer_unshift(hbu_buffer_t buf, hb_char_t head) {
  hbu_buffer_append(buf, '\0'); // Use append instead of ensure_length to grow rather than crawl

  for (size_t i = 0; i < buf->length - 1; i++) { // -1 to skip last appended '\0'
    buf->data[i + 1] = buf->data[i];
  }

  buf->data[0] = head;
  // length already adjusted thanks to initial append
}

hb_eod_char_t hbu_buffer_pop(hbu_buffer_t buf) {
  if (buf->length == 0) {
    return HB_EOD;
  }

  size_t idx = buf->length - 1;

  hb_char_t l = buf->data[idx];

  buf->data[idx] = '\0';
  buf->length--;

  return l;
}

int hbu_buffer_compare(hbu_buffer_t a, hbu_buffer_t b) {
  // All buffers have NULL-terminated underlying char arrays
  return strcmp(a->data, b->data);
}

int hbu_buffer_equal(hbu_buffer_t a, hbu_buffer_t b) {
  return a->length == b->length &&
         hbu_buffer_compare(a, b);
}

#endif
