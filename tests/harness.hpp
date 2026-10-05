// harness.hpp -- the tiny test framework both of us use. No dependency, no
// download, header-only.
//
// Created by Anas (Week 1), maintained by Alyan. It is FROZEN after Week 1:
// nobody edits tests to fit a feature -- a test changes only with a note in
// its PR.
//
// Using STL containers here is allowed and encouraged: tests compare our
// structures against std::vector / std::unordered_map as an oracle. The ban on
// STL containers applies to engine code under src/, not to tests/.
//
// Usage:
//   #include "harness.hpp"
//   PLAG_TEST(my_case) { CHECK(1 + 1 == 2); }
// CMake defines PLAG_HARNESS_MAIN for every test target, which supplies main().
#ifndef PLAG_TESTS_HARNESS_HPP_
#define PLAG_TESTS_HARNESS_HPP_

#include <cmath>
#include <cstddef>
#include <cstdio>
#include <string>
#include <vector>

namespace plagtest {

struct TestCase {
  const char* name;
  void (*fn)();
};

inline std::vector<TestCase>& registry() {
  static std::vector<TestCase> r;
  return r;
}

struct Counters {
  int checks = 0;
  int failures = 0;
  const char* current = "";
};

inline Counters& counters() {
  static Counters c;
  return c;
}

struct Registrar {
  Registrar(const char* name, void (*fn)()) { registry().push_back({name, fn}); }
};

inline void fail(const char* file, int line, const std::string& what) {
  ++counters().failures;
  std::printf("  FAIL %s:%d  [%s]\n       %s\n", file, line,
              counters().current, what.c_str());
}

inline std::string describe(const char* expr) { return std::string(expr); }

template <class A, class B>
void check_eq(const A& a, const B& b, const char* exprA, const char* exprB,
              const char* file, int line) {
  ++counters().checks;
  if (!(a == b)) {
    ++counters().failures;
    std::printf("  FAIL %s:%d  [%s]\n       %s != %s\n", file, line,
                counters().current, exprA, exprB);
  }
}

inline void check_near(double a, double b, double eps, const char* expr,
                       const char* file, int line) {
  ++counters().checks;
  if (!(std::fabs(a - b) <= eps)) {
    ++counters().failures;
    std::printf("  FAIL %s:%d  [%s]\n       %s: %g vs %g (eps %g)\n", file, line,
                counters().current, expr, a, b, eps);
  }
}

inline int run_all(const char* suite) {
  std::printf("== %s: %zu test case(s)\n", suite, registry().size());
  for (const TestCase& t : registry()) {
    counters().current = t.name;
    const int before = counters().failures;
    t.fn();
    std::printf("  %s %s\n", counters().failures == before ? "ok  " : "FAIL",
                t.name);
  }
  const bool green = counters().failures == 0;
  std::printf("== %s: %d check(s), %d failure(s) -> %s\n", suite,
              counters().checks, counters().failures, green ? "PASS" : "FAIL");
  return green ? 0 : 1;
}

}  // namespace plagtest

#define PLAG_CONCAT_INNER(a, b) a##b
#define PLAG_CONCAT(a, b) PLAG_CONCAT_INNER(a, b)

// Registers a test case. The file name is the suite name in the output.
#define PLAG_TEST(name)                                                       \
  static void PLAG_CONCAT(plag_test_fn_, name)();                            \
  static ::plagtest::Registrar PLAG_CONCAT(plag_test_reg_, name)(            \
      #name, &PLAG_CONCAT(plag_test_fn_, name));                             \
  static void PLAG_CONCAT(plag_test_fn_, name)()

#define CHECK(expr)                                                          \
  do {                                                                       \
    ++::plagtest::counters().checks;                                         \
    if (!(expr)) {                                                           \
      ::plagtest::fail(__FILE__, __LINE__, "CHECK(" #expr ")");              \
    }                                                                        \
  } while (false)

#define CHECK_EQ(a, b)                                                       \
  ::plagtest::check_eq((a), (b), #a, #b, __FILE__, __LINE__)

#define CHECK_NEAR(a, b, eps)                                                \
  ::plagtest::check_near(static_cast<double>(a), static_cast<double>(b),     \
                         static_cast<double>(eps), #a " ~= " #b, __FILE__,   \
                         __LINE__)

#define FAIL_MSG(msg) ::plagtest::fail(__FILE__, __LINE__, (msg))

// Marks a test that exists to hold the place of a step not yet built. It
// prints the step it belongs to so the log is honest about what is stubbed.
#define PLAG_STUB_NOTE(step)                                                 \
  std::printf("  note: STUB -- real assertions land in %s\n", (step))

#ifdef PLAG_HARNESS_MAIN
int main() { return ::plagtest::run_all(PLAG_TEST_SUITE); }
#endif

#endif  // PLAG_TESTS_HARNESS_HPP_
