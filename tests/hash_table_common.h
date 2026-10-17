// Shared test cases for both chaining and open-addressing hash tables
#ifndef PLAG_TESTS_HASH_TABLE_COMMON_H_
#define PLAG_TESTS_HASH_TABLE_COMMON_H_

#include <cstdint>
#include <unordered_map>

#include "harness.hpp"

namespace plagtest {

template <typename Table>
void shared_empty_table_lookup() {
  Table t;
  CHECK_EQ(t.size(), std::size_t{0});
  CHECK(t.bucketCount() > 0);
  CHECK(t.find(12345ULL) == nullptr);
}

template <typename Table>
void shared_insert_then_find() {
  Table t;
  std::unordered_map<std::uint64_t, int> oracle;

  for (int i = 1; i <= 5000; ++i) {
    std::uint64_t k = static_cast<std::uint64_t>(i) * 2654435761ULL;
    t.insert(k, i * 10);
    oracle[k] = i * 10;
  }

  CHECK_EQ(t.size(), oracle.size());

  for (const auto& kv : oracle) {
    const int* found = t.find(kv.first);
    CHECK(found != nullptr);
    if (found != nullptr) {
      CHECK_EQ(*found, kv.second);
    }
  }

  CHECK(t.find(999999999ULL) == nullptr);
}

template <typename Table>
void shared_grow_while_inserting() {
  Table t;
  std::size_t initial_buckets = t.bucketCount();

  for (int i = 0; i < 200; ++i) {
    t.insert(static_cast<std::uint64_t>(i + 1), i);
  }

  CHECK(t.bucketCount() > initial_buckets);
  CHECK_EQ(t.size(), std::size_t{200});

  for (int i = 0; i < 200; ++i) {
    int* v = t.find(static_cast<std::uint64_t>(i + 1));
    CHECK(v != nullptr);
    if (v != nullptr) {
      CHECK_EQ(*v, i);
    }
  }
}

template <typename Table>
void shared_adversarial_low_bit_keys() {
  Table t;
  for (std::uint64_t i = 1; i <= 256; ++i) {
    std::uint64_t key = i << 32;  // keys with identical low 32 bits
    t.insert(key, static_cast<int>(i));
  }

  CHECK_EQ(t.size(), std::size_t{256});
  CHECK(t.longestChain() < 32);

  for (std::uint64_t i = 1; i <= 256; ++i) {
    int* val = t.find(i << 32);
    CHECK(val != nullptr);
    if (val != nullptr) {
      CHECK_EQ(*val, static_cast<int>(i));
    }
  }
}

template <typename Table>
void shared_erase_and_reinsert() {
  Table t;
  for (std::uint64_t i = 1; i <= 50; ++i) {
    t.insert(i, static_cast<int>(i * 2));
  }
  CHECK_EQ(t.size(), std::size_t{50});

  CHECK(t.erase(10ULL));
  CHECK(!t.erase(10ULL));
  CHECK(t.find(10ULL) == nullptr);
  CHECK_EQ(t.size(), std::size_t{49});

  t.insert(10ULL, 777);
  int* v = t.find(10ULL);
  CHECK(v != nullptr);
  if (v != nullptr) {
    CHECK_EQ(*v, 777);
  }
}

template <typename Table>
void run_shared_hash_table_suite(const char* /*variant*/) {
  shared_empty_table_lookup<Table>();
  shared_insert_then_find<Table>();
  shared_grow_while_inserting<Table>();
  shared_adversarial_low_bit_keys<Table>();
  shared_erase_and_reinsert<Table>();
}

}  // namespace plagtest

#endif  // PLAG_TESTS_HASH_TABLE_COMMON_H_
