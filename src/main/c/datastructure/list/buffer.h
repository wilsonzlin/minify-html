#ifndef _HDR_HYPERBUILD_DATASTRUCTURE_LIST_BUFFER
#define _HDR_HYPERBUILD_DATASTRUCTURE_LIST_BUFFER

#include "../../util/math.h"
#include "../../util/buffer.c"
#include "../../util/list.h"

HBU_LIST(hb_bufferlist, hbu_buffer_t, sizeof(hbu_buffer_t), hbu_buffer_t, NULL);

int hb_bufferlist_compare(hb_bufferlist_t a, hb_bufferlist_t b)
{
  size_t max = hbu_max(a->length, b->length);

  for (size_t i = 0; i < max; i++)
  {
    hbu_buffer_t a1 = hb_bufferlist_get(a, i);
    hbu_buffer_t b1 = hb_bufferlist_get(b, i);

    if (a1 == NULL)
    {
      return -1;
    }

    if (b1 == NULL)
    {
      return 1;
    }

    int subcmp = hbu_buffer_compare(a1, b1);
    if (subcmp != 0)
    {
      return subcmp;
    }
  }

  return 0;
}

hb_bufferlist_t hb_bufferlist_create_from_split(hb_char_t *source, hb_char_t delim)
{
  hb_bufferlist_t parts = hb_bufferlist_create();
  hbu_buffer_t part = hbu_buffer_create();
  hb_bufferlist_append(parts, part);

  hb_char_t c;
  size_t i = 0;
  while ((c = source[i]))
  {
    if (c == delim)
    {
      part = hbu_buffer_create();
      hb_bufferlist_append(parts, part);
    }
    else
    {
      hbu_buffer_append(part, c);
    }

    i++;
  }

  return parts;
}

void hb_bufferlist_destroy_from_split(hb_bufferlist_t list)
{
  for (size_t i = 0; i < list->length; i++) {
    hbu_buffer_destroy(hb_bufferlist_get(list, i));
  }
  hb_bufferlist_destroy(list);
}

#endif // _HDR_HYPERBUILD_DATASTRUCTURE_LIST_BUFFER
