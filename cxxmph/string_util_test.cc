#include "string_util.h"
#include "test.h"

using namespace cxxmph;

bool test_format() {
  string expected = " %% 4 foo 0x0A bar ";
  string foo = "foo";
  string fmt = format(" %%%% %v %v 0x%.2X bar ", 4, foo, 10);
  fail_unless(fmt == expected, "expected\n-%s-\n got \n-%s-", expected.c_str(), fmt.c_str());
  return true;
}

bool test_infoln() {
  infoln(string("%s:%d: MY INFO LINE"), __FILE__, __LINE__);
  return true;
}


bool test_macro() {
  CXXMPH_DEBUGLN("here i am")();
  return true;
}

CXXMPH_TEST_CASE(test_format)
CXXMPH_TEST_CASE(test_infoln)
CXXMPH_TEST_CASE(test_macro)
