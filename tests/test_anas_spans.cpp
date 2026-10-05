// test_anas_spans.cpp -- scattered hits -> disjoint maximal copied spans.
// Owner: Anas.  Step 7c (Week 5).
//
// STUB: merge_spans() emits nothing yet.
#include <cstddef>

#include "common/dyn_array.hpp"
#include "contract/span.hpp"
#include "index/span_merge.hpp"
#include "harness.hpp"

PLAG_TEST(merge_spans_stub_produces_no_spans) {
  plag::Span hits[2] = {};
  plag::DynArray<plag::Span> out;
  const std::size_t n = plag::merge_spans(hits, 2, 0, out);
  CHECK_EQ(n, std::size_t{0});
  CHECK_EQ(out.size(), std::size_t{0});
  PLAG_STUB_NOTE("Step 7c -- span merging");
}

PLAG_TEST(hits_one_token_apart_merge_into_one_span) {
  // TODO(Anas W5 Step 7c): (0..10 | 0..10) and (10..11 | 10..11) merge into
  // 0..11 in both documents.
  PLAG_STUB_NOTE("Step 7c -- adjacency merge");
  CHECK(true);
}

PLAG_TEST(hits_with_a_different_offset_in_b_stay_separate) {
  // A different offset in B means a different copied block: it must NOT merge.
  // TODO(Anas W5 Step 7c).
  PLAG_STUB_NOTE("Step 7c -- offset mismatch");
  CHECK(true);
}

PLAG_TEST(merged_spans_equal_a_naive_longest_common_substring_scan) {
  // 500 random corpora, merged spans compared against a naive LCS scan.
  // TODO(Anas W5 Step 7c).
  PLAG_STUB_NOTE("Step 7c -- naive oracle cross-check");
  CHECK(true);
}
