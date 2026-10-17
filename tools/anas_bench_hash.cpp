// Benchmark comparing separate chaining vs linear probing hash tables
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "index/hash_table.hpp"
#include "index/hash_table_open.hpp"

namespace {

double elapsed_ms(std::chrono::steady_clock::time_point start) {
  auto diff = std::chrono::steady_clock::now() - start;
  return std::chrono::duration<double, std::milli>(diff).count();
}

template <typename Table>
void run_benchmark(const char* name, std::size_t n) {
  std::vector<std::uint64_t> keys(n);
  std::uint64_t state = 1337ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    keys[i] = state;
  }

  Table table;

  auto t0 = std::chrono::steady_clock::now();
  for (std::size_t i = 0; i < n; ++i) {
    table.insert(keys[i], static_cast<int>(i));
  }
  double insert_time = elapsed_ms(t0);

  auto t1 = std::chrono::steady_clock::now();
  std::size_t found_count = 0;
  for (std::size_t i = 0; i < n; ++i) {
    if (table.find(keys[i]) != nullptr) {
      ++found_count;
    }
  }
  double lookup_time = elapsed_ms(t1);

  std::printf("| %s | %.1f ms | %.1f ms | %.2f | %zu | %zu (hits: %zu) |\n",
              name, insert_time, lookup_time, table.meanChainLength(),
              table.longestChain(), table.bucketCount(), found_count);
}

}  // namespace

int main(int argc, char** argv) {
  std::size_t n = 100000;
  if (argc > 1) {
    long parsed = std::atol(argv[1]);
    if (parsed > 0) {
      n = static_cast<std::size_t>(parsed);
    }
  }

  std::printf("Hash Table Benchmark (%zu keys)\n\n", n);
  std::printf("| Variant | Insert Time | Lookup Time | Avg Chain/Probe | Max | Buckets |\n");
  std::printf("|---|---|---|---|---|---|\n");

  run_benchmark<plag::HashTable<std::uint64_t, int>>("Chaining", n);
  run_benchmark<plag::OpenHashTable<std::uint64_t, int>>("Linear Probing", n);

  return 0;
}
