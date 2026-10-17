// Unit tests for span_merge
#include <cstddef>

#include "common/dyn_array.hpp"
#include "contract/span.hpp"
#include "index/span_merge.hpp"
#include "harness.hpp"

PLAG_TEST(merge_empty_spans) {
  plag::DynArray<plag::Span> out;
  std::size_t n = plag::merge_spans(nullptr, 0, 0, out);
  CHECK_EQ(n, std::size_t{0});
  CHECK(out.empty());
}

PLAG_TEST(adjacent_and_overlapping_spans_merge) {
  plag::Span hits[3] = {
      {0, 0, 5, 1, 10, 15},
      {0, 1, 6, 1, 11, 16},
      {0, 6, 10, 1, 16, 20},
  };
  plag::DynArray<plag::Span> out;
  std::size_t n = plag::merge_spans(hits, 3, 0, out);

  CHECK_EQ(n, std::size_t{1});
  CHECK_EQ(out[0].startA, 0u);
  CHECK_EQ(out[0].endA, 10u);
  CHECK_EQ(out[0].startB, 10u);
  CHECK_EQ(out[0].endB, 20u);
}

PLAG_TEST(spans_with_different_offsets_stay_separate) {
  plag::Span hits[2] = {
      {0, 0, 5, 1, 0, 5},
      {0, 5, 10, 1, 20, 25},
  };
  plag::DynArray<plag::Span> out;
  std::size_t n = plag::merge_spans(hits, 2, 0, out);

  CHECK_EQ(n, std::size_t{2});
  CHECK_EQ(out[0].tokens(), 5u);
  CHECK_EQ(out[1].tokens(), 5u);
}
