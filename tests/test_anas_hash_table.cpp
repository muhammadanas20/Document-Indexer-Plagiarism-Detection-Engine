// Unit tests for separate-chaining and open-addressing hash tables
#include <cstdint>

#include "contract/contract.hpp"
#include "hash_table_common.h"
#include "index/hash_table.hpp"
#include "index/hash_table_open.hpp"

using ChainingTable = plag::HashTable<std::uint64_t, int>;
using ProbingTable = plag::OpenHashTable<std::uint64_t, int>;

PLAG_TEST(chaining_table_passes_shared_suite) {
  plagtest::run_shared_hash_table_suite<ChainingTable>("chaining");
}

PLAG_TEST(open_addressing_table_passes_shared_suite) {
  plagtest::run_shared_hash_table_suite<ProbingTable>("open_addressing");
}

PLAG_TEST(hash_table_with_occurrence_list_values) {
  plag::HashTable<std::uint64_t, plag::OccurrenceList> index;
  plag::OccurrenceList list;
  list.push_back(plag::Occurrence{0, 12});
  list.push_back(plag::Occurrence{1, 34});

  index.insert(999ULL, list);
  const plag::OccurrenceList* found = index.find(999ULL);
  CHECK(found != nullptr);
  if (found != nullptr) {
    CHECK_EQ(found->size(), std::size_t{2});
    CHECK_EQ((*found)[0].docId, 0u);
    CHECK_EQ((*found)[1].pos, 34u);
  }
}
