// test_util.hpp  --  a very small test helper (no external framework)
//
// How it works: CHECK(condition) counts one check and prints a line when the
// condition is false. At the end of main we call testSummary("name"), which
// returns 0 when everything passed, so ctest sees a normal exit code.
//
// We print something for every failed check, which is why ctest
// --output-on-failure shows exactly what broke.

#ifndef DIP_TEST_UTIL_HPP
#define DIP_TEST_UTIL_HPP

#include <cmath>
#include <iostream>

namespace testutil {
inline int& checks() {
  static int value = 0;
  return value;
}
inline int& failures() {
  static int value = 0;
  return value;
}
inline void record(bool ok, const char* text, int line, const char* file) {
  ++checks();
  if (!ok) {
    ++failures();
    std::cout << "  FAILED: " << text << "   (" << file << ":" << line << ")\n";
  }
}
inline int summary(const char* name) {
  std::cout << name << ": " << (checks() - failures()) << "/" << checks()
            << " checks passed\n";
  if (failures() > 0) {
    std::cout << name << ": " << failures() << " CHECK(S) FAILED\n";
    return 1;
  }
  return 0;
}
}  // namespace testutil

#define CHECK(condition) \
  testutil::record((condition), #condition, __LINE__, __FILE__)

#define CHECK_NEAR(actual, expected, tolerance)                            \
  testutil::record(std::fabs((actual) - (expected)) <= (tolerance),        \
                   #actual " ~= " #expected, __LINE__, __FILE__)

#endif  // DIP_TEST_UTIL_HPP
