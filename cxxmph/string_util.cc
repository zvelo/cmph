#include "string_util.h"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>

using namespace std;

namespace cxxmph {

bool stream_printf(
    const std::string& format_string, uint32_t offset, std::ostream* out) {
  if (offset == format_string.length()) return true;
  assert(offset < format_string.length());
  cerr << "length:" << format_string.length() << endl;
  cerr << "offset:" << offset << endl;
  auto txt = format_string.substr(offset, format_string.length() - offset);
  *out << txt;
  return true;
}

}  // namespace cxxmph
