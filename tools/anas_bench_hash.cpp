// anas_bench_hash.cpp -- the chaining-vs-probing benchmark.
// Owner: Anas (Step 7b, Week 5). Output is pasted into
// docs/benchmarks-anas.md, and every number quoted in the report has a line
// there.
//
// STUB: both tables are dummies until Step 5 / Step 7a, so the timings below
// are meaningless and the table says so. The measurement scaffolding is real.
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "index/hash_table.hpp"
#include "index/hash_table_open.hpp"

namespace {

class Rng {
 public:
  explicit Rng(std::uint64_t seed) : s_(seed | 1ULL) {}
  std::uint64_t next() {
    s_ ^= s_ >> 12;
    s_ ^= s_ << 25;
    s_ ^= s_ >> 27;
    return s_ * 2685821657736338717ULL;
  }
 private:
  std::uint64_t s_;
};

double ms_since(std::chrono::steady_clock::time_point t0) {
  const auto d = std::chrono::steady_clock::now() - t0;
  return std::chrono::duration<double, std::milli>(d).count();
}

// TODO(Anas W5 Step 7b): read VmHWM from /proc/self/status on Linux for peak
// RSS; fall back to ru_maxrss from getrusage() elsewhere.
double peak_rss_mb() { return 0.0; }

template <class Table>
void bench(const char* variant, std::size_t n, std::uint64_t seed) {
  std::vector<std::uint64_t> keys(n);
  Rng rng(seed);
  for (std::size_t i = 0; i < n; ++i) keys[i] = rng.next();

  Table t;

  const auto t0 = std::chrono::steady_clock::now();
  for (std::size_t i = 0; i < n; ++i) t.insert(keys[i], static_cast<int>(i));
  const double insert_ms = ms_since(t0);

  const auto t1 = std::chrono::steady_clock::now();
  volatile std::size_t hits = 0;
  for (std::size_t i = 0; i < n; ++i) {
    if (t.find(keys[i]) != nullptr) ++hits;
  }
  const double lookup_ms = ms_since(t1);

  // NOTE: chain and probe stats are only comparable once both tables are
  // implemented; today both report 0.
  std::printf("| %s | %.0f ms | %.0f ms | %.0f MB | %.2f | %zu | %zu |\n",
              variant, insert_ms, lookup_ms, peak_rss_mb(), 0.0,
              std::size_t{0}, t.bucketCount());
}

}  // namespace

int main(int argc, char** argv) {
  std::size_t n = 1000000;
  if (argc > 1) n = static_cast<std::size_t>(std::atol(argv[1]));

  std::printf("<!-- tools/anas_bench_hash.cpp %zu keys, paste into "
              "docs/benchmarks-anas.md -->\n", n);
  std::printf("| variant | inserts | lookups | peak RSS | mean chain/probe | "
              "max | buckets |\n");
  std::printf("|---|---|---|---|---|---|---|\n");

  bench<plag::HashTable<std::uint64_t, int>>("chaining", n, 11);
  bench<plag::OpenHashTable<std::uint64_t, int>>("linear probing", n, 11);

  std::printf("\nSTUB: both tables are unimplemented (Step 5 / Step 7a), so the "
              "numbers above are placeholders, not results.\n");
  return 0;
}
