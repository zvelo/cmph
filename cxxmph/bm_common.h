#ifndef __CXXMPH_BM_COMMON_H__
#define __CXXMPH_BM_COMMON_H__

#include "stringpiece.h"

#include <string>
#include <vector>
#include <unordered_map>  // std::hash
#include "MurmurHash3.h"

#include "benchmark.h"

namespace std {
template <> struct hash<cxxmph::StringPiece> {
  uint32_t operator()(const cxxmph::StringPiece& k) const {
    uint32_t out;
    MurmurHash3_x86_32(k.data(), k.length(), 1, &out);
    return out;
  }
};
}  // namespace std

namespace cxxmph {

class UrlsBenchmark : public Benchmark {
 public:
  UrlsBenchmark(const std::string& urls_file) : urls_file_(urls_file) { }
  virtual  ~UrlsBenchmark();
 protected:
  virtual bool SetUp();
  const std::string urls_file_;
  std::vector<std::string> urls_;
};

class SearchUrlsBenchmark : public UrlsBenchmark {
 public:
  SearchUrlsBenchmark(const std::string& urls_file, uint32_t nsearches, float miss_ratio)
      : UrlsBenchmark(urls_file), nsearches_(nsearches), miss_ratio_(miss_ratio) {}
 virtual ~SearchUrlsBenchmark();
 protected:
  virtual bool SetUp();
  const uint32_t nsearches_; 
  float miss_ratio_;
  std::vector<std::string> forced_miss_urls_;
  std::vector<StringPiece> random_;
};

class Uint64Benchmark : public Benchmark {
 public:
  Uint64Benchmark(uint32_t count) : count_(count) { }
  virtual ~Uint64Benchmark();
  virtual void Run() {}
 protected:
  virtual bool SetUp();
  const uint32_t count_;
  std::vector<uint64_t> values_;
};

class SearchUint64Benchmark : public Uint64Benchmark {
 public:
  SearchUint64Benchmark(uint32_t count, uint32_t nsearches)
      : Uint64Benchmark(count), nsearches_(nsearches) { }
  virtual ~SearchUint64Benchmark();
  virtual void Run() {};
 protected:
  virtual bool SetUp();
  const uint32_t nsearches_;
  std::vector<uint64_t> random_;
};

}  // namespace cxxmph

#endif  // __CXXMPH_BM_COMMON_H__
