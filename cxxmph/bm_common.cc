#include <cmath>
#include <fstream>
#include <limits>
#include <iostream>
#include <set>

#include "bm_common.h"

using std::cerr;
using std::endl;
using std::set;
using std::string;
using std::vector;

namespace cxxmph {
  
UrlsBenchmark::~UrlsBenchmark() {}
bool UrlsBenchmark::SetUp() {
  vector<string> urls;
  std::ifstream f(urls_file_.c_str());
  if (!f.is_open()) {
    cerr << "Failed to open urls file " << urls_file_ << endl;
    return false;
  }
  string buffer;
  while(std::getline(f, buffer)) urls.push_back(buffer);
  set<string> unique(urls.begin(), urls.end());
  if (unique.size() != urls.size()) {
    cerr << "Input file has repeated keys." << endl;
    return false;
  }
  urls.swap(urls_);
  return true;
}

SearchUrlsBenchmark::~SearchUrlsBenchmark() {}
bool SearchUrlsBenchmark::SetUp() {
  if (!UrlsBenchmark::SetUp()) return false;
  int32_t miss_ratio_int32 = std::numeric_limits<int32_t>::max() * miss_ratio_;
  forced_miss_urls_.resize(nsearches_);
  random_.resize(nsearches_);
  for (uint32_t i = 0; i < nsearches_; ++i) {
    random_[i] = urls_[random() % urls_.size()];
    if (random() < miss_ratio_int32) {
      forced_miss_urls_[i] = random_[i].as_string() + ".force_miss";
      random_[i] = forced_miss_urls_[i];
    }
  }
  return true;
}

Uint64Benchmark::~Uint64Benchmark() {}
bool Uint64Benchmark::SetUp() {
  set<uint64_t> unique;
  for (uint32_t i = 0; i < count_; ++i) {
    uint64_t v;
    do { v = random(); } while (unique.find(v) != unique.end());
    values_.push_back(v);
    unique.insert(v);
  }
  return true;
}

SearchUint64Benchmark::~SearchUint64Benchmark() {}
bool SearchUint64Benchmark::SetUp() {
  if (!Uint64Benchmark::SetUp()) return false;
  random_.resize(nsearches_);
  for (uint32_t i = 0; i < nsearches_; ++i) {
    uint32_t pos = random() % values_.size();
    random_[i] = values_[pos];
  }
  return true;
}

}  // namespace cxxmph 
