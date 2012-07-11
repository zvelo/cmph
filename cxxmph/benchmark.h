#ifndef __CXXMPH_BENCHMARK_H__
#define __CXXMPH_BENCHMARK_H__

#include <string>
#include <typeinfo>

namespace cxxmph {

class Benchmark {
 public:
  Benchmark() {}
  virtual ~Benchmark() {}

  const std::string& name() { return name_; }
  void set_name(const std::string& name) { name_ = name; }

  static void Register(Benchmark* bm);
  static void RunAll();

 protected:
  virtual bool SetUp() { return true; }; 
  virtual void Run() = 0;
  virtual bool TearDown() { return true; };

 private:
  std::string name_;
  void MeasureRun();
};

}  // namespace cxxmph

#endif
