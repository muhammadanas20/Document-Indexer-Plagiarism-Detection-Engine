#ifndef DIP_TEST_UTIL_HPP
#define DIP_TEST_UTIL_HPP

#include <cmath>
#include <iostream>

inline int& checks() {
  static int value = 0;
  return value;
}
inline int& failures() {
  static int value = 0;
  return value;
}
inline void recordCheck(bool ok, const char* text, int line, const char* file) {
  ++checks();
  if (!ok) {
    ++failures();
    std::cout << "  FAILED: " << text << "   (" << file << ":" << line << ")\n";
  }
}
inline int testSummary(const char* name) {
  std::cout << name << ": " << (checks() - failures()) << "/" << checks()
            << " checks passed\n";
  if (failures() > 0) {
    std::cout << name << ": " << failures() << " CHECK(S) FAILED\n";
    return 1;
  }
  return 0;
}

#define CHECK(condition) \
  recordCheck((condition), #condition, __LINE__, __FILE__)

#define CHECK_NEAR(actual, expected, tolerance)                            \
  recordCheck(std::fabs((actual) - (expected)) <= (tolerance),        \
                   #actual " ~= " #expected, __LINE__, __FILE__)

#endif
