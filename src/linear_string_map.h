// A simple linked list based dynamic sized associative map from const char* to
// void*. Designed to maximize ease of use instead of performance. Should be
// used in benchmarks and tests only, not to be distributed with the cmph
// runtime headers.

typedef struct __linear_string_map_t lsmap_t;

lsmap_t *lsmap_new();
void lsmap_append(lsmap_t *lsmap, const char *key, void *value);
void* lsmap_search(lsmap_t *lsmap, const char *key);
void lsmap_foreach_key(lsmap_t* lsmap, void (*f)(const char*));
void lsmap_foreach_value(lsmap_t* lsmap, void (*f)(void*));
void lsmap_destroy(lsmap_t* lsmap);
