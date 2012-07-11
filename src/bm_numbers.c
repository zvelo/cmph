#include <stdlib.h>
#include <string.h>

#include "bitbool.h"
#include "cmph.h"
#include "cmph_benchmark.h"
#include "linear_string_map.h"

// Generates a vector with random unique 32 bits integers
cmph_uint32* random_numbers_vector_new(cmph_uint32 size) {
  cmph_uint32 i = 0;
  cmph_uint32 dup_bits = sizeof(cmph_uint32)*size*8;
  char* dup = (char*)malloc(dup_bits/8);
  cmph_uint32* vec = (cmph_uint32 *)malloc(sizeof(cmph_uint32)*size);
  memset(dup, 0, dup_bits/8);
  for (i = 0; i < size; ++i) {
    cmph_uint32 v = random();
    while (GETBIT(dup, v % dup_bits)) { v = random(); }
    SETBIT(dup, v % dup_bits);
    vec[i] = v;
  }
  free(dup);
  return vec;
}

int cmph_uint32_cmp(const void *a, const void *b) { 
  return *(const cmph_uint32*)a - *(const cmph_uint32*)b;
}

char* create_lsmap_key(CMPH_ALGO algo, int iters) {
  char mphf_name[128];
  snprintf(mphf_name, 128, "%s:%u", cmph_names[algo], iters);
  return strdup(mphf_name);
}

static cmph_uint32 g_numbers_len = 0;
static cmph_uint32 *g_numbers = NULL;
static lsmap_t *g_created_mphf = NULL;
static lsmap_t *g_expected_probes = NULL;
static lsmap_t *g_mphf_probes = NULL;

void bm_create(CMPH_ALGO algo, int iters) {
  cmph_io_adapter_t* source = NULL;
  cmph_config_t* config = NULL;
  cmph_t* mphf = NULL;

  if (iters > (int)g_numbers_len) {
    fprintf(stderr, "No input with proper size.");
    exit(-1);
  }

  source = cmph_io_struct_vector_adapter(
      (void*)g_numbers, sizeof(cmph_uint32),
      0, sizeof(cmph_uint32), iters);
  config = cmph_config_new(source);
  cmph_config_set_algo(config, algo);
  mphf = cmph_new(config);
  if (!mphf) {
    fprintf(stderr, "Failed to create mphf for algorithm %s with %u keys",
            cmph_names[algo], iters);
    exit(-1);
  }
  cmph_config_destroy(config);
  cmph_io_struct_vector_adapter_destroy(source);
  lsmap_append(g_created_mphf, create_lsmap_key(algo, iters), mphf);
}

void bm_search(CMPH_ALGO algo, int iters) {
  int i = 0;
  char *mphf_name;
  cmph_t* mphf = NULL; 

  mphf_name = create_lsmap_key(algo, iters);
  mphf = (cmph_t*)lsmap_search(g_created_mphf, mphf_name);
  free(mphf_name);

  cmph_uint32* count = (cmph_uint32*)malloc(sizeof(cmph_uint32)*iters);  
  cmph_uint32* hash_count = (cmph_uint32*)malloc(sizeof(cmph_uint32)*iters);  

  for (i = 0; i < iters * 100; ++i) {
    cmph_uint32 pos = random() % iters;
    const char* buf = (const char*)(g_numbers + pos);
    cmph_uint32 h = cmph_search(mphf, buf, sizeof(cmph_uint32));
    ++count[pos];
    ++hash_count[h];
  }

  // Verify correctness later.
  lsmap_append(g_expected_probes, create_lsmap_key(algo, iters), count);
  lsmap_append(g_mphf_probes, create_lsmap_key(algo, iters), hash_count);
}

void verify() { }

#define DECLARE_ALGO(algo) \
  void bm_create_ ## algo(int iters) { bm_create(algo, iters); } \
  void bm_search_ ## algo(int iters) { bm_search(algo, iters); }

DECLARE_ALGO(CMPH_BMZ);
DECLARE_ALGO(CMPH_CHM);
DECLARE_ALGO(CMPH_BRZ);
DECLARE_ALGO(CMPH_FCH);
DECLARE_ALGO(CMPH_BDZ);

int main(int argc, char** argv) {
  g_numbers_len = 1000 * 1000;
  g_numbers = random_numbers_vector_new(g_numbers_len);
  g_created_mphf = lsmap_new();
  g_expected_probes = lsmap_new();
  g_mphf_probes = lsmap_new();

  BM_REGISTER(bm_create_CMPH_BMZ, 1000 * 1000);
  BM_REGISTER(bm_search_CMPH_BMZ, 1000 * 1000);
  BM_REGISTER(bm_create_CMPH_CHM, 1000 * 1000);
  BM_REGISTER(bm_search_CMPH_CHM, 1000 * 1000);
//  BM_REGISTER(bm_create_CMPH_BRZ, 1000 * 1000);
//  BM_REGISTER(bm_search_CMPH_BRZ, 1000 * 1000);
//  BM_REGISTER(bm_create_CMPH_FCH, 1000 * 1000);
//  BM_REGISTER(bm_search_CMPH_FCH, 1000 * 1000);
  BM_REGISTER(bm_create_CMPH_BDZ, 1000 * 1000);
  BM_REGISTER(bm_search_CMPH_BDZ, 1000 * 1000);
  run_benchmarks(argc, argv);

  verify();
  free(g_numbers);
  lsmap_foreach_key(g_created_mphf, (void(*)(const char*))free);
  lsmap_foreach_value(g_created_mphf, (void(*)(void*))cmph_destroy);
  lsmap_destroy(g_created_mphf);
  return 0;
}
