#ifndef __CXXMPH_TEST_H__
#define __CXXMPH_TEST_H__

// Thin wrapper on top of check.h to get rid of boilerplate in tests. Assumes a
// single test suite and test case per file, with each fixture represented by a
// parameter-less boolean function.
//
// The check.h header macro-clashes with c++ libraries so this file needs to be
// included last.

#include <check.h>

extern Suite* global_suite;
extern TCase* global_tc_core;

// Creates a new test case calling boolean_function. Name must be a valid,
// unique c identifier when prefixed with tc_.
#define CXXMPH_CXX_TEST_CASE(name, boolean_function) \
  START_TEST(tc_ ## name) \
       { fail_unless(boolean_function()); } END_TEST \
  static TestCase global_cxxmph_tc_ ## name(tc_ ## name);

#define CXXMPH_TEST_CASE(name) CXXMPH_CXX_TEST_CASE(name, name)

struct TestCase {
  TestCase(void (*f)(int)) {
    tcase_add_test(global_tc_core, f);
  }
};

#endif //  __CXXMPH_TEST_H__
