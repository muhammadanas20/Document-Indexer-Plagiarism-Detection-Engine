// test_anas_hash_table.cpp -- the graded centrepiece's tests.
// Owner: Anas.  Step 5 (Week 3). Runs the SHARED suite from
// hash_table_common.h so the Week 5 probing variant cannot get an easier test.
//
// STUB: the table is not implemented yet. These cases assert only the stub
// contract so ctest stays green from Week 0; the real assertions are marked
// TODO and land with Step 5.
#include <cstdint>

#include "contract/contract.hpp"
#include "hash_table_common.h"
#include "index/hash_table.hpp"

using ChainingTable = plag::HashTable<std::uint64_t, int>;

PLAG_TEST(chaining_table_passes_the_shared_suite) {
  plagtest::run_shared_hash_table_suite<ChainingTable>("chaining");
}

PLAG_TEST(chaining_table_index_of_mixes_high_bits) {
  ChainingTable t;
  CHECK(t.bucketCount() >= 16);
  CHECK_EQ(t.size(), std::size_t{0});
  CHECK_NEAR(t.meanChainLength(), 0.0, 1e-12);
  PLAG_STUB_NOTE("Step 5 -- insert/find/grow assertions");
}

PLAG_TEST(chaining_table_is_templated_so_alyan_can_reuse_it) {
  // The contract: it works for (docA,docB) -> int, not just fingerprints.
  // If this does not compile, Step 5 is not done.
  plag::HashTable<std::uint64_t, int> counter;
  plag::HashTable<std::uint64_t, plag::OccurrenceList> index;
  CHECK_EQ(counter.size(), std::size_t{0});
  CHECK_EQ(index.size(), std::size_t{0});
}
