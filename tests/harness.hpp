// Simple lightweight unit testing header used across test suites
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
  Registrar(const char* name, void (*fn)()) {
    registry().push_back({name, fn});
  }
};

inline void fail(const char* file, int line, const std::string& what) {
  ++counters().failures;
  std::printf("  FAIL %s:%d [%s]: %s\n",
              file, line, counters().current, what.c_str());
}

template <typename A, typename B>
void check_eq(const A& a, const B& b, const char* exprA, const char* exprB,
              const char* file, int line) {
  ++counters().checks;
  if (!(a == b)) {
    ++counters().failures;
    std::printf("  FAIL %s:%d [%s]: %s != %s\n",
                file, line, counters().current, exprA, exprB);
  }
}

inline void check_near(double a, double b, double eps, const char* expr,
                       const char* file, int line) {
  ++counters().checks;
  if (std::fabs(a - b) > eps) {
    ++counters().failures;
    std::printf("  FAIL %s:%d [%s]: %s (%g vs %g)\n",
                file, line, counters().current, expr, a, b);
  }
}

inline int run_all(const char* suite) {
  std::printf("== %s (%zu tests)\n", suite, registry().size());
  for (const TestCase& t : registry()) {
    counters().current = t.name;
    int before = counters().failures;
    t.fn();
    std::printf("  %s %s\n",
                (counters().failures == before) ? "PASS" : "FAIL", t.name);
  }
  bool ok = (counters().failures == 0);
  std::printf("== %s: %d checks, %d failures -> %s\n",
              suite, counters().checks, counters().failures,
              ok ? "PASS" : "FAIL");
  return ok ? 0 : 1;
}

}  // namespace plagtest

#define PLAG_JOIN_INNER(a, b) a##b
#define PLAG_JOIN(a, b) PLAG_JOIN_INNER(a, b)

#define PLAG_TEST(name)                                                  \
  static void PLAG_JOIN(test_fn_, name)();                               \
  static ::plagtest::Registrar PLAG_JOIN(test_reg_, name)(               \
      #name, &PLAG_JOIN(test_fn_, name));                                \
  static void PLAG_JOIN(test_fn_, name)()

#define CHECK(expr)                                                      \
  do {                                                                   \
    ++::plagtest::counters().checks;                                     \
    if (!(expr)) {                                                       \
      ::plagtest::fail(__FILE__, __LINE__, "CHECK(" #expr ")");          \
    }                                                                    \
  } while (false)

#define CHECK_EQ(a, b)                                                   \
  ::plagtest::check_eq((a), (b), #a, #b, __FILE__, __LINE__)

#define CHECK_NEAR(a, b, eps)                                            \
  ::plagtest::check_near(static_cast<double>(a), static_cast<double>(b), \
                         static_cast<double>(eps), #a " ~= " #b,         \
                         __FILE__, __LINE__)

#define FAIL_MSG(msg) ::plagtest::fail(__FILE__, __LINE__, (msg))

#ifdef PLAG_HARNESS_MAIN
int main() { return ::plagtest::run_all(PLAG_TEST_SUITE); }
#endif

#endif  // PLAG_TESTS_HARNESS_HPP_
