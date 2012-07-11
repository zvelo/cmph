#include "benchmark.h"

#include <cerrno>
#include <cstring>
#include <cstdio>
#include <memory>
#include <sys/time.h>
#include <sys/resource.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using std::cerr;
using std::cout;
using std::endl;
using std::setfill;
using std::setw;
using std::string;
using std::ostringstream;
using std::vector;

namespace {

/* Subtract the `struct timeval' values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0.  */
int timeval_subtract ( 
    struct timeval *result, struct timeval *x, struct timeval* y) {
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}

// C++ iostream is terrible for formatting.
string timeval_to_string(timeval tv) {
  ostringstream out;
  out << setfill(' ') << setw(3) << tv.tv_sec << '.';
  out << setfill('0') << setw(6) << tv.tv_usec;
  return out.str();
}

struct rusage getrusage_or_die() {
  struct rusage rs;
  int ret = getrusage(RUSAGE_SELF, &rs);
  if (ret != 0) {
    cerr << "rusage failed: " << strerror(errno) << endl;
    exit(-1);
  }
  return rs;
}

struct timeval gettimeofday_or_die() {
  struct timeval tv;
  int ret = gettimeofday(&tv, NULL); 
  if (ret != 0) {
    cerr << "gettimeofday failed: " << strerror(errno) << endl;
    exit(-1);
  }
  return tv;
}

#ifdef HAVE_CXA_DEMANGLE
string demangle(const string& name) {
  char buf[1024];
  unsigned int size = 1024;
  int status;
  char* res = abi::__cxa_demangle(
     name.c_str(), buf, &size, &status);
  return res;
}
#else
string demangle(const string& name) { return name; }
#endif
 

static vector<cxxmph::Benchmark*> g_benchmarks;

}  // anonymous namespace

namespace cxxmph {

/* static */ void Benchmark::Register(Benchmark* bm) {
  if (bm->name().empty()) {
    string name = demangle(typeid(*bm).name());
    bm->set_name(name);
  }
  g_benchmarks.push_back(bm);
}

/* static */ void Benchmark::RunAll() {
  for (uint32_t i = 0; i < g_benchmarks.size(); ++i) {
    std::auto_ptr<Benchmark> bm(g_benchmarks[i]);
    if (!bm->SetUp()) {
      cerr << "Set up phase for benchmark "
           << bm->name() << " failed." << endl;
      continue;
    }
    bm->MeasureRun();
    bm->TearDown(); 
  }
}

void Benchmark::MeasureRun() {
  struct timeval walltime_begin = gettimeofday_or_die();
  struct rusage begin = getrusage_or_die();
  Run();
  struct rusage end = getrusage_or_die();
  struct timeval walltime_end = gettimeofday_or_die();

  struct timeval utime;
  timeval_subtract(&utime, &end.ru_utime, &begin.ru_utime);
  struct timeval stime;
  timeval_subtract(&stime, &end.ru_stime, &begin.ru_stime);
  struct timeval wtime;
  timeval_subtract(&wtime, &walltime_end, &walltime_begin);

  cout << "Benchmark: " << name_ << endl;
  cout << "CPU User time  : " << timeval_to_string(utime) << endl;
  cout << "CPU System time: " << timeval_to_string(stime) << endl;
  cout << "Wall clock time: " << timeval_to_string(wtime) << endl;
  cout << endl;
}

}  // namespace cxxmph
