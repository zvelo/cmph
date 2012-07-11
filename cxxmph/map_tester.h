#ifndef __CXXMPH_MAP_TEST_HELPER_H__
#define __CXXMPH_MAP_TEST_HELPER_H__

#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

#include "string_util.h"
#include <check.h>

namespace cxxmph {

using namespace cxxmph;
using namespace std;

template <template<typename...> class map_type>
struct MapTester {
  static bool small_insert() {
    map_type<int64_t, int64_t> m;
    // Start counting from 1 to not touch default constructed value bugs
    for (int i = 1; i < 12; ++i) m.insert(make_pair(i, i));
    return m.size() == 11;
  }
  static bool large_insert() {
    map_type<int64_t, int64_t> m;
    // Start counting from 1 to not touch default constructed value bugs
    int nkeys = 12 * 256 * 256;
    for (int i = 1; i < nkeys; ++i) m.insert(make_pair(i, i));
    return static_cast<int>(m.size()) == nkeys - 1;
  }
  static bool small_search() {
    map_type<int64_t, int64_t> m;
    // Start counting from 1 to not touch default constructed value bugs
    for (int i = 1; i < 12; ++i) m.insert(make_pair(i, i));
    for (int i = 1; i < 12; ++i) if (m.find(i) == m.end()) return false; 
    return true;
  }
  static bool default_search() {
    map_type<int64_t, int64_t> m;
    if (m.find(0) != m.end()) return false;
    for (int i = 1; i < 256; ++i) m.insert(make_pair(i, i));
    if (m.find(0) != m.end()) return false;
    for (int i = 0; i < 256; ++i) m.insert(make_pair(i, i));
    if (m.find(0) == m.end()) return false;
    return true;
  }
  static bool large_search() {
    int nkeys = 10 * 1000;
    map_type<int64_t, int64_t> m;
    for (int i = 0; i < nkeys; ++i) m.insert(make_pair(i, i));
    for (int i = 0; i < nkeys; ++i) if (m.find(i) == m.end()) return false; 
    return true;
  }
  static bool string_search() {
    int nkeys = 10 * 1000;
    vector<string> keys;
    for (int i = 0; i < nkeys; ++i) {
      keys.push_back(format("%v", i));
    }
    map_type<string, int64_t> m;
    for (int i = 0; i < nkeys; ++i) m.insert(make_pair(keys[i], i));
    for (int i = 0; i < nkeys; ++i) {
      auto it = m.find(keys[i]);
      if (it == m.end()) return false;
      if (it->second != i) return false;
    }
    return true;
  }
  static bool rehash_zero() {
    map_type<int64_t, int64_t> m;
    m.rehash(0);
    return m.size() == 0;
  }
  static bool rehash_size() {
    map_type<int64_t, int64_t> m;
    int nkeys = 10 * 1000;
    for (int i = 0; i < nkeys; ++i) { m.insert(make_pair(i, i)); }
    m.rehash(nkeys);
    for (int i = 0; i < nkeys; ++i) { if (m.find(i) == m.end()) return false; }
    for (int i = nkeys; i < nkeys * 2; ++i) {
      if (m.find(i) != m.end()) return false;
    }
    return true;
  }
  static bool erase_iterator() {
    map_type<int64_t, int64_t> m;
    int nkeys = 10 * 1000;
    for (int i = 0; i < nkeys; ++i) { m.insert(make_pair(i, i)); }
    for (int i = 0; i < nkeys; ++i) {
       if (m.find(i) == m.end()) return false;
    }
    for (int i = nkeys - 1; i >= 0; --i) { if (m.find(i) == m.end()) return false; }
    for (int i = nkeys - 1; i >= 0; --i) {
      fail_unless(m.find(i) != m.end(), "after erase %d cannot be found", i);
      fail_unless(m.find(i)->first == i, "after erase key %d cannot be found", i);
    }
    for (int i = nkeys - 1; i >= 0; --i) {
      fail_unless(m.find(i) != m.end(), "after erase %d cannot be found", i);
      fail_unless(m.find(i)->first == i, "after erase key %d cannot be found", i);
      if (!(m.find(i)->first == i)) return false;
      m.erase(m.find(i));
      if (static_cast<int>(m.size()) != i) return false;
    }
    return true;
 }
 static bool erase_value() {
    map_type<int64_t, int64_t> m;
    int nkeys = 10 * 1000;
    for (int i = 0; i < nkeys; ++i) { m.insert(make_pair(i, i)); }
    for (int i = nkeys - 1; i >= 0; --i) { 
      fail_unless(m.find(i) != m.end());
      m.erase(i);
      if (static_cast<int>(m.size()) != i) return false;
    }
    return true;
  }
};

}  // namespace cxxxmph

#endif // __CXXMPH_MAP_TEST_HELPER_H__
