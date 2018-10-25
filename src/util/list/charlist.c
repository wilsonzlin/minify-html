#include "../math/math.c"
#include "./__base__.c"
#include "./char.c"

HBU_LIST(hbu_list_charlist, hbu_list_char_t, sizeof(hbu_list_char_t), hbu_list_char_t, NULL);

int hbu_list_charlist_compare(hbu_list_charlist_t a, hbu_list_charlist_t b)
{
  size_t max = hbu_max(a->length, b->length);

  for (size_t i = 0; i < max; i++)
  {
    hbu_list_char_t a1 = hbu_list_charlist_get(a, i);
    hbu_list_char_t b1 = hbu_list_charlist_get(b, i);

    if (a1 == NULL)
    {
      return -1;
    }

    if (b1 == NULL)
    {
      return 1;
    }

    int subcmp = hbu_list_char_compare(a1, b1);
    if (subcmp != 0)
    {
      return subcmp;
    }
  }

  return 0;
}

hbu_list_charlist_t hbu_list_charlist_create_from_split(hb_char_t *source, hb_char_t delim)
{
  hbu_list_charlist_t parts = hbu_list_charlist_create();
  hbu_list_char_t part = hbu_list_char_create();
  hbu_list_charlist_append(parts, part);

  hb_char_t c;
  size_t i = 0;
  while ((c = source[i]))
  {
    if (c == delim)
    {
      part = hbu_list_char_create();
      hbu_list_charlist_append(parts, part);
    }
    else
    {
      hbu_list_char_append(part, c);
    }

    i++;
  }

  return parts;
}

void hbu_list_charlist_destroy_from_split(hbu_list_charlist_t list)
{
  for (size_t i = 0; i < list->length; i++) {
    hbu_list_char_destroy(hbu_list_charlist_get(list, i));
  }
  hbu_list_charlist_destroy(list);
}
