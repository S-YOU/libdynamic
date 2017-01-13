#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "buffer.h"
#include "vector.h"
#include "map.h"

static size_t map_roundup(size_t s)
{
  s --;
  s |= s >> 1;
  s |= s >> 2;
  s |= s >> 4;
  s |= s >> 8;
  s |= s >> 16;
  s |= s >> 32;
  s ++;

  return s;
}

static void map_release_all(map *m, int (*equal)(void *, void *), void (*release)(void *))
{
  size_t i;

  if (release)
    for (i = 0; i < m->elements_capacity; i ++)
      if (!equal((char *) m->elements + (i * m->element_size), m->element_empty))
        release((char *) m->elements + (i * m->element_size));
}

static void *map_element(map *m, size_t position)
{
  return (char *) m->elements + (position * m->element_size);
}

static void map_rehash(map *m, size_t size, size_t (*hash)(void *), int (*equal)(void *, void *))
{
  map new;
  size_t i;

  size = map_roundup(size);
  new = *m;
  new.elements_count = 0;
  new.elements_capacity = size;
  new.elements = malloc(new.elements_capacity * new.element_size);
  for (i = 0; i < new.elements_capacity; i ++)
    memcpy(map_element(&new, i), new.element_empty, new.element_size);
  if (m->elements)
    {
      for (i = 0; i < m->elements_capacity; i ++)
        if (!equal(map_element(m, i), m->element_empty))
          map_insert(&new, map_element(m, i), hash, equal, NULL);
      free(m->elements);
    }
  *m = new;
}

/* constructor/destructor */

void map_construct(map *m, size_t element_size, void *element_empty)
{
  m->elements = NULL;
  m->elements_count = 0;
  m->elements_capacity = 0;
  m->element_size = element_size;
  m->element_empty = element_empty;
  map_rehash(m, MAP_ELEMENTS_CAPACITY_MIN, NULL, NULL);
}

void map_destruct(map *m, int (*equal)(void *, void *), void (*release)(void *))
{
  map_release_all(m, equal, release);
  free(m->elements);
}

/* capacity */

size_t map_size(map *m)
{
  return m->elements_count;
}

void map_reserve(map *m, size_t size, size_t (*hash)(void *), int (*equal)(void *, void *))
{
  size *= 2;
  if (size > m->elements_capacity)
    map_rehash(m, size, hash, equal);
}

/* element access */

void *map_element_empty(map *m)
{
  return m->element_empty;
}

void *map_at(map *m, void *element, size_t (*hash)(void *), int (*equal)(void *, void *))
{
  size_t i;
  void *test;

  i = hash(element);
  while (1)
    {
      i &= m->elements_capacity - 1;
      test = map_element(m, i);
      if (equal(test, element) || equal(test, m->element_empty))
        return test;
      i ++;
    }
}

/* modifiers */

void map_insert(map *m, void *element, size_t (*hash)(void *), int (*equal)(void *, void *), void (*release)(void *))
{
  void *test;

  map_reserve(m, m->elements_count + 1, hash, equal);
  test = map_at(m, element, hash, equal);
  if (equal(test, m->element_empty))
    {
      memcpy(test, element, m->element_size);
      m->elements_count ++;
    }
  else
    {
      if (release)
        release(element);
    }
}

void map_erase(map *m, void *element, size_t (*hash)(void *), int (*equal)(void *, void *), void (*release)(void *))
{
  void *test;
  size_t i, j, k;

  test = map_at(m, element, hash, equal);
  if (equal(test, m->element_empty))
    return;

  if (release)
    release(test);
  m->elements_count --;

  i = ((char *) test - (char *) m->elements) / m->element_size;
  j = i;
  while (1)
    {
      j = (j + 1) & (m->elements_capacity - 1);
      if (equal(map_element(m, j), m->element_empty))
        break;

      k = hash(map_element(m, j)) & (m->elements_capacity - 1);
      if ((i < j && (k <= i || k > j)) ||
          (i > j && (k <= i && k > j)))
        {
          memcpy(map_element(m, i), map_element(m, j), m->element_size);
          i = j;
        }
    }

  memcpy(map_element(m, i), m->element_empty, m->element_size);
}

void map_clear(map *m, int (*equal)(void *, void *), void (*release)(void *))
{
  map_release_all(m, equal, release);
  free(m->elements);
  m->elements = NULL;
  m->elements_count = 0;
  m->elements_capacity = 0;
  map_rehash(m, MAP_ELEMENTS_CAPACITY_MIN, NULL, NULL);
}
