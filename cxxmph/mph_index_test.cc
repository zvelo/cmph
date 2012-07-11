#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

#include "mph_index.h"

using std::string;
using std::vector;
using namespace cxxmph;

int main(int argc, char** argv) {

  srand(1);
  vector<string> keys;
  keys.push_back("davi");
  keys.push_back("paulo");
  keys.push_back("joao");
  keys.push_back("maria");
  keys.push_back("bruno");
  keys.push_back("paula");
  keys.push_back("diego");
  keys.push_back("diogo");
  keys.push_back("algume");

  SimpleMPHIndex<string> mph_index;
  if (!mph_index.Reset(keys.begin(), keys.end(), keys.size())) { exit(-1); }
  vector<int> ids;
  for (vector<int>::size_type i = 0; i < keys.size(); ++i) {
     ids.push_back(mph_index.index(keys[i]));
     cerr << " " << *(ids.end() - 1);
  }
  cerr << endl;
  sort(ids.begin(), ids.end());
  for (vector<int>::size_type i = 0; i < ids.size(); ++i) assert(ids[i] == static_cast<vector<int>::value_type>(i));

  FlexibleMPHIndex<false, true, int64_t, seeded_hash<std::hash<int64_t>>::hash_function> square_empty;
  auto id = square_empty.index(1);
  FlexibleMPHIndex<false, false, int64_t, seeded_hash<std::hash<int64_t>>::hash_function> unordered_empty;
  id ^= unordered_empty.index(1);
  FlexibleMPHIndex<true, false, int64_t, seeded_hash<std::hash<int64_t>>::hash_function> minimal_empty;
  id ^= minimal_empty.index(1);
}
