#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "linear_string_map.h"

struct __linear_string_map_t {
  const char *key;
  void *value;
  struct __linear_string_map_t* next;
};

lsmap_t *lsmap_new() {
  lsmap_t* lsmap = (lsmap_t*)malloc(sizeof(lsmap_t));
  if (!lsmap) return NULL;
  lsmap->key = "dummy node";
  lsmap->next = NULL;
  return lsmap;
}

int lsmap_size(lsmap_t *lsmap) {
  int size = 0;
  while (lsmap->next != NULL) ++size;
  return size;
}

void lsmap_append(lsmap_t *lsmap, const char *key, void *value) {
  while (lsmap->next != NULL) lsmap = lsmap->next;
  lsmap->next = (lsmap_t*)malloc(sizeof(lsmap_t));
  lsmap->key = key;
  lsmap->value = value;
  lsmap = lsmap->next;
  lsmap->key = "dummy node";
  lsmap->next = NULL;
}

void* lsmap_search(lsmap_t *lsmap, const char *key) {
  while (lsmap->next != NULL) {
    if (strcmp(lsmap->key, key) == 0) {
      return lsmap->value;
    }
    lsmap = lsmap->next;
  }
  return NULL;
}

void lsmap_foreach_key(lsmap_t *lsmap, void (*f)(const char*)) {
  while (lsmap->next != NULL) {
    f(lsmap->key);
    lsmap = lsmap->next;
  }
}

void lsmap_foreach_value(lsmap_t *lsmap, void (*f)(void*)) {
  while (lsmap->next != NULL) {
    f(lsmap->value);
    lsmap = lsmap->next;
  }
}

void lsmap_destroy(lsmap_t *lsmap) {
  while (lsmap->next != NULL) {
    lsmap_t* freeme = lsmap;
    lsmap = lsmap->next;
    free(freeme);
  }
  free(lsmap);
}
