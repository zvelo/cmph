#include <unistd.h>  // for sleep
#include <limits.h>

#include "cmph_benchmark.h"

void bm_sleep(int iters) {
  sleep(1);
}

void bm_increment(int iters) {
  int i, v = 0;
  for (i = 0; i < INT_MAX; ++i) {
    v += i;
  }
}

int main(int argc, char** argv) {
  BM_REGISTER(bm_sleep, 1);
  BM_REGISTER(bm_increment, 1);
  run_benchmarks(argc, argv);
  return 0;
}

