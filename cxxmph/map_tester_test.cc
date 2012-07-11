#include "map_tester.h"
#include "test.h"

using namespace cxxmph;

typedef MapTester<std::unordered_map> Tester;

CXXMPH_CXX_TEST_CASE(small_insert, Tester::small_insert);
CXXMPH_CXX_TEST_CASE(large_insert, Tester::large_insert);
CXXMPH_CXX_TEST_CASE(small_search, Tester::small_search);
CXXMPH_CXX_TEST_CASE(default_search, Tester::default_search);
CXXMPH_CXX_TEST_CASE(large_search, Tester::large_search);
CXXMPH_CXX_TEST_CASE(string_search, Tester::string_search);
CXXMPH_CXX_TEST_CASE(rehash_zero, Tester::rehash_zero);
CXXMPH_CXX_TEST_CASE(rehash_size, Tester::rehash_size);
CXXMPH_CXX_TEST_CASE(erase_value, Tester::erase_value);
CXXMPH_CXX_TEST_CASE(erase_iterator, Tester::erase_iterator);
