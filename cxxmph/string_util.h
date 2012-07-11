#ifndef __CXXMPH_STRING_UTIL_H__
#define __CXXMPH_STRING_UTIL_H__

// Helper functions for string formatting and terminal output. Should be used
// only for debugging and tests, since performance was not a concern.
// Implemented using variadic templates because it is cool.
//
// Adds the extra format %v to the printf formatting language. Uses the method
// cxxmph::tostr to implement custom printers and fallback to operator
// ostream::operator<< otherwise.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

#define CXXMPH_DEBUGLN(fmt) variadic_print(__FILE__, __LINE__, &std::cerr, fmt)
#define CXXMPH_INFOLN(fmt) variadic_print(__FILE__, __LINE__, &std::cout, fmt)

namespace cxxmph {

using std::pair;
using std::string;
using std::ostream;
using std::vector;

template <class T> void tostr(ostream *out, const T& v) {
  *out << v;
}
inline void tostr(std::ostream* out, uint8_t v) {
  *out << static_cast<uint32_t>(v);
}
template <class V>
inline void tostr(ostream* out, const vector<V>& v) {
  *out << "[";
  for (uint32_t i = 0; i < v.size(); ++i) {
    tostr(out, v[1]);
    if (i != v.size() - 1)*out << " ";
  }
  *out << "]";
}
template <class F, class S>
inline void tostr(ostream* out, const pair<F, S>& v) {
  *out << "(";
  tostr(out, v.first);
  *out << ",";
  tostr(out, v.second);
  *out << ")";
}

bool stream_printf(
    const std::string& format_string, uint32_t offset, std::ostream* out);

template <bool ispod> struct pod_snprintf {};
template <> struct pod_snprintf<false> {
  template <class T>
  int operator()(char*, size_t, const char*, const T&) {
     return -1;
  }
};
template <> struct pod_snprintf<true> {
  template <class T>
  int operator()(char* str, size_t size, const char* format, const T& v) {
    return snprintf(str, size, format, v);
  }
};

template <typename T, typename... Args>
bool stream_printf(const std::string& format_string, uint32_t offset,
                   std::ostream* out, const T& value, Args&&... args) {
  auto txt = format_string.c_str() + offset;
  while (*txt) {
    auto b = txt;
    for (; *txt != '%'; ++txt);
    if (*(txt + 1) == '%') ++txt;
    else if (txt == b) break;
    *out << string(b, txt - b);
    if (*(txt - 1) == '%') ++txt;
  }
  auto fmt = txt + 1;
  while (*fmt && *fmt != '%') ++fmt;
  if (strncmp(txt, "%v", 2) == 0) {
     txt += 2;
     tostr(out, value);
     if (txt != fmt) *out << string(txt, fmt);
  } else {
    char buf[256];  // Is this enough?
    auto n = pod_snprintf<std::is_pod<T>::value>()(
        buf, 256, std::string(txt, fmt).c_str(), value);
    if (n < 0) return false;
    *out << buf;
  }
  return stream_printf(format_string, fmt - format_string.c_str(), out,
                       std::forward<Args>(args)...);
}

template <typename... Args>
std::string format(const std::string& format_string, Args&&... args) {
  std::ostringstream out;
  if (!stream_printf(format_string, 0, &out, std::forward<Args>(args)...)) {
    return std::string();
  };
  return out.str();
}

template <typename... Args>
void infoln(const std::string& format_string, Args&&... args) {
  stream_printf(format_string + "\n", 0, &std::cout, std::forward<Args>(args)...);
}

struct variadic_print {
  variadic_print(const std::string& file, uint32_t line, std::ostream* out,
                 const std::string& format_line) 
     : file_(file), line_(line), out_(out), format_line_(format_line) {}
  template <typename... Args>
  void operator()(Args&&... args) {
    std::string fancy_format = "%v:%d: ";
    fancy_format += format_line_ + "\n";
    stream_printf(fancy_format, 0, out_, file_, line_, std::forward<Args>(args)...);
  }
  const std::string& file_;
  const uint32_t& line_;
  std::ostream* out_;
  const std::string& format_line_;
};

}  // namespace cxxmph

#endif // __CXXMPH_STRING_UTIL_H__
