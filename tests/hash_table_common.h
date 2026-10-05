// hash_table_common.h -- the SHARED suite both hash table variants must pass.
// Owner: Anas.  Written in Week 3 (Step 5) so the open-addressing variant of
// Week 5 (Step 7a) is run through exactly the same checks instead of getting a
// friendlier test. Templated on the table type: that is what makes it shared.
//
// The full bodies land with Step 5. Until then each case asserts only the
// documented stub contract, and says so out loud.
#ifndef PLAG_TESTS_HASH_TABLE_COMMON_H_
#define PLAG_TESTS_HASH_TABLE_COMMON_H_

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "common/dyn_array.hpp"
#include "harness.hpp"

namespace plagtest {

// Deterministic xorshift64* -- the suite must give identical output on every
// run, on every machine, or it cannot be quoted in the report.
class HashRng {
 public:
  explicit HashRng(std::uint64_t seed) : s_(seed | 1ULL) {}
  std::uint64_t next() {
    s_ ^= s_ >> 12;
    s_ ^= s_ << 25;
    s_ ^= s_ >> 27;
    return s_ * 2685821657736338717ULL;
  }
 private:
  std::uint64_t s_;
};

// --- Step 5 / Step 7a shared cases -----------------------------------------
// Each function is a real test body once the table it is instantiated with is
// implemented. The oracle is std::unordered_map, which is allowed in tests/.

template <class Table>
void shared_empty_table_lookup() {
  Table t;
  CHECK_EQ(t.size(), std::size_t{0});
  CHECK(t.bucketCount() > 0);
  CHECK(t.find(12345ULL) == nullptr);   // find on an empty table must not crash
  PLAG_STUB_NOTE("Step 5 (chaining) / Step 7a (open addressing)");
}

template <class Table>
void shared_insert_then_find() {
  Table t;
  // TODO(Anas W3): insert 1M random keys, compare every lookup against a
  // std::unordered_map<uint64_t,int> oracle built in parallel.
  PLAG_STUB_NOTE("Step 5 (chaining) / Step 7a (open addressing)");
  CHECK_EQ(t.size(), std::size_t{0});
}

template <class Table>
void shared_grow_while_inserting() {
  Table t;
  const std::size_t buckets0 = t.bucketCount();
  // TODO(Anas W3): insert past the load-factor threshold and assert the
  // bucket count doubled and every key is still findable after the rehash.
  PLAG_STUB_NOTE("Step 5 (chaining) / Step 7a (open addressing)");
  CHECK(t.bucketCount() >= buckets0);
}

template <class Table>
void shared_adversarial_low_bit_keys() {
  Table t;
  // TODO(Anas W3): keys that share their low bits (k << 32). With the high-bit
  // mix in place, longestChain must stay <= ~2x meanChainLength. Without it,
  // every key lands in bucket 0 -- that is the bug this test exists to catch.
  PLAG_STUB_NOTE("Step 5 (chaining) / Step 7a (open addressing)");
  CHECK_EQ(t.longestChain(), std::size_t{0});
}

// Run every shared case against one table type. Call it from
// tests/test_anas_hash_table.cpp (Step 5) and again from the open-addressing
// test in Step 7a -- identical checks, different table.
template <class Table>
void run_shared_hash_table_suite(const char* /*variant*/) {
  shared_empty_table_lookup<Table>();
  shared_insert_then_find<Table>();
  shared_grow_while_inserting<Table>();
  shared_adversarial_low_bit_keys<Table>();
}

}  // namespace plagtest

#endif  // PLAG_TESTS_HASH_TABLE_COMMON_H_
