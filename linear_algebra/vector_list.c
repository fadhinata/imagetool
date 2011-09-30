#include <stdio.h>
#include <assert.h>

#include <vector.h>
#include <dlink.h>
#include <vector_list.h>

void vector_list_copy(vector_list_t *copyer, vector_list_t *copee)
{
  dlink_t *a, *b;

  assert(copyer);
  assert(copee);
  assert(vector_list_get_count(copyer) == vector_list_get_count(copee));

  a = copyer->tail->next;
  for (b = copee->tail->next; b != copee->head; b = b->next) {
    vector_copy((vector_t *)a->object, (vector_t *)b->object);
    a = a->next;
  }
}

vector_list_t *vector_list_new_and_copy(vector_list_t *list)
{
  dlink_t *a, *b;
  vector_list_t *ret;
  vector_t *vec;

  assert(list);

  ret = vector_list_new();

  for (a = list->tail->next; a != list->head; a = a->next) {
    vec = vector_new_and_copy((vector_t *)a->object);
    vector_inc_ref(vec);
    b = dlink_new();
    b->object = (void *)vec;
    dlist_insert(b, (dlist_t *)ret);
  }

  return ret;
}

void vector_list_push(vector_t *vec, vector_list_t *list)
{
  dlink_t *link;

  assert(vec);
  assert(list);

  vector_inc_ref(vec);
  link = dlink_new();
  link->object = (void *)vec;
  dlist_push(link, list);
}

vector_t *vector_list_pop(vector_list_t *list)
{
  dlink_t *link;
  vector_t *vec;

  assert(list);

  link = dlist_pop((dlist_t *)list);
  vec = (vector_t *)link->object;
  vector_dec_ref(vec);
  dlink_destroy(link);

  return vec;
}

void vector_list_insert(vector_t *vec, vector_list_t *list)
{
  dlink_t *link;

  assert(vec);
  assert(list);

  vector_inc_ref(vec);
  link = dlink_new();
  link->object = (void *)vec;
  dlist_insert(link, list);
}

vector_t *vector_list_extract(vector_list_t *list)
{
  dlink_t *link;
  vector_t *vec;

  assert(list);

  link = dlist_extract((dlist_t *)list);
  vec = (vector_t *)link->object;
  vector_dec_ref(vec);
  dlink_destroy(link);

  return vec;
}

vector_t *vector_list_glimpse(int i, vector_list_t *list)
{
  dlink_t *link;
  vector_t *vec;

  assert(list);
  assert(0 <= i && i < vector_list_get_count(list));

  link = dlist_glimpse(i, (dlist_t *)list);
  vec = (vector_t *)link->object;

  return vec;
}

vector_t *vector_list_pick(int i, vector_list_t *list)
{
  dlink_t *link;
  vector_t *vec;

  assert(list);
  assert(0 <= i && i < vector_list_get_count(list));

  link = dlist_pick(i, (dlist_t *)list);
  vec = (vector_t *)link->object;
  vector_dec_ref(vec);
  dlink_destroy(link);

  return vec;
}

void vector_list_put(vector_t *vec, int i, vector_list_t *list)
{
  dlink_t *link;

  assert(vec);
  assert(list);
  assert(0 <= i && i < vector_list_get_count(list));

  vector_inc_ref(vec);
  link = dlink_new();
  link->object = (void *)vec;
  dlist_put(link, i, list);
}

int vector_list_query(vector_t *p, vector_list_t *list)
{
  int i;
  dlink_t *x;

  assert(p);
  assert(list);

  i = 0;
  for (x = list->tail->next; x != list->head; x = x->next) {
    if (vector_cmp(p, (vector_t *)x->object) == 0)
      return i;
    i++;
  }

  return -1;
}

void vector_list_delete(vector_list_t *list)
{
  dlink_t *link;
  vector_t *p;

  assert(list);

  for (link = list->tail->next; link != list->head; link = link->next) {
    p = (vector_t *)link->object;
    vector_dec_ref(p);
  }
}

void vector_list_destroy(vector_list_t *list)
{
  dlink_t *link;

  assert(list);

  if (vector_list_get_ref(list) <= 0) {
    while (vector_list_get_count(list) > 0) {
      link = dlist_pop((dlist_t *)list);
      vector_destroy((vector_t *)link->object);
      dlink_destroy(link);
    }
    dlist_destroy((dlist_t *)list);
  } else {
    vector_list_dec_ref(list);
  }
}
