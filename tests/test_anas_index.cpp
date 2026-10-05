// test_anas_index.cpp -- IndexBuilder and the API Alyan codes against.
// Owner: Anas.  Step 6 (Week 4). This is the hard deadline: Alyan's Week 5
// candidate finder compiles against lookup() from Sun 8 Nov.
//
// STUB: the index is empty; lookup() returns nullptr with count 0.
#include <cstdint>

#include "index/index_builder.hpp"
#include "harness.hpp"

PLAG_TEST(lookup_on_an_empty_index_returns_null_with_zero_count) {
  plag::IndexBuilder idx;
  std::size_t count = 999;
  const plag::Occurrence* occ = idx.lookup(0xdeadbeefULL, &count);
  CHECK(occ == nullptr);
  CHECK_EQ(count, std::size_t{0});
}

PLAG_TEST(stats_start_at_zero) {
  plag::IndexBuilder idx;
  const plag::IndexStats& st = idx.stats();
  CHECK_EQ(st.files, std::size_t{0});
  CHECK_EQ(st.tokens, std::size_t{0});
  CHECK_EQ(st.kgrams, std::size_t{0});
  CHECK_EQ(st.distinct, std::size_t{0});
  CHECK_NEAR(st.ms, 0.0, 1e-12);
}

PLAG_TEST(two_identical_documents_share_every_kgram) {
  // Correctness in one line: two identical fixture files -> every k-gram hash
  // of theirs reports >= 2 occurrences; a unique file reports exactly 1.
  // TODO(Anas W4 Step 6): real assertion once add_document() works.
  plag::IndexBuilder idx;
  const std::uint64_t ids[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  idx.add_document(0, "a.txt", ids, 8, 4);
  idx.add_document(1, "b.txt", ids, 8, 4);

  std::size_t count = 0;
  CHECK(idx.lookup(12345ULL, &count) == nullptr);   // STUB: index still empty
  CHECK_EQ(idx.stats().files, std::size_t{0});      // STUB: no counters yet
  PLAG_STUB_NOTE("Step 6 -- shared k-gram occurrence counts");
}

PLAG_TEST(document_view_exposes_norm_tokens_and_pos_to_line) {
  // The two things Alyan's report needs: normalised token ids and line numbers.
  plag::IndexBuilder idx;
  const plag::DocumentView& dv = idx.doc(0);
  CHECK_EQ(dv.norm.size(), std::size_t{0});
  CHECK_EQ(dv.posToLine.size(), std::size_t{0});
  PLAG_STUB_NOTE("Step 6 -- per-document pools retained for the report");
}

PLAG_TEST(indexing_50k_tokens_is_inside_the_time_budget) {
  // Budget: < 200 ms in debug, < 20 ms in release; memory <= 6 bytes/token.
  // Measured numbers go into docs/benchmarks-anas.md in the same PR.
  // TODO(Anas W4 Step 6): time it with a real corpus and record the number.
  PLAG_STUB_NOTE("Step 6 -- timing and bytes-per-token measurement");
  CHECK(true);
}
