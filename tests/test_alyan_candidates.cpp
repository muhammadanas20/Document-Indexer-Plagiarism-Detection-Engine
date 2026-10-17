// Unit tests for candidate pair generation
#include <cstdint>

#include "index/index_builder.hpp"
#include "search/candidate_generator.hpp"
#include "harness.hpp"

PLAG_TEST(empty_index_emits_no_candidates) {
  plag::IndexBuilder idx;
  plag::DynArray<plag::CandidatePair> out;
  CHECK_EQ(plag::generate_candidates(idx, 1, out), std::size_t{0});
  CHECK(out.empty());
}

PLAG_TEST(identical_documents_are_candidates) {
  plag::IndexBuilder idx;
  const std::uint64_t ids[6] = {10, 20, 30, 40, 50, 60};
  idx.add_document(0, "a.txt", ids, 6, 3);
  idx.add_document(1, "b.txt", ids, 6, 3);

  plag::DynArray<plag::CandidatePair> out;
  std::size_t count = plag::generate_candidates(idx, 1, out);
  CHECK_EQ(count, std::size_t{1});
  CHECK_EQ(out[0].docA, 0u);
  CHECK_EQ(out[0].docB, 1u);
  CHECK_EQ(out[0].sharedKgrams, 4u);
}

PLAG_TEST(disjoint_documents_are_not_candidates) {
  plag::IndexBuilder idx;
  const std::uint64_t docA[5] = {1, 2, 3, 4, 5};
  const std::uint64_t docB[5] = {101, 102, 103, 104, 105};
  idx.add_document(0, "a.txt", docA, 5, 3);
  idx.add_document(1, "b.txt", docB, 5, 3);

  plag::DynArray<plag::CandidatePair> out;
  CHECK_EQ(plag::generate_candidates(idx, 1, out), std::size_t{0});
}
