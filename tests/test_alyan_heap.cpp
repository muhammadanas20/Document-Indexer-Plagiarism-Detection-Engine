// Unit tests for MaxHeap
#include "rank/max_heap.hpp"
#include "search/scorer.hpp"
#include "harness.hpp"

namespace {

plag::PairScore make_pair(plag::DocumentId a, plag::DocumentId b, double cov) {
  plag::PairScore p;
  p.docA = a;
  p.docB = b;
  p.jaccard = cov;
  p.containment = cov;
  p.coverage = cov;
  p.matchedTokens = static_cast<std::uint32_t>(cov * 100.0);
  return p;
}

}  // namespace

PLAG_TEST(empty_heap_properties) {
  plag::MaxHeap h;
  CHECK(h.empty());
  CHECK_EQ(h.size(), std::size_t{0});
}

PLAG_TEST(push_and_pop_descending_order) {
  plag::MaxHeap h;
  h.push(make_pair(0, 1, 0.25));
  h.push(make_pair(0, 2, 0.90));
  h.push(make_pair(1, 2, 0.55));
  h.push(make_pair(2, 3, 0.75));

  CHECK_EQ(h.size(), std::size_t{4});
  CHECK_NEAR(h.top().coverage, 0.90, 1e-9);

  plag::PairScore p1 = h.pop();
  plag::PairScore p2 = h.pop();
  plag::PairScore p3 = h.pop();
  plag::PairScore p4 = h.pop();

  CHECK(plag::suspicion_key(p1) >= plag::suspicion_key(p2));
  CHECK(plag::suspicion_key(p2) >= plag::suspicion_key(p3));
  CHECK(plag::suspicion_key(p3) >= plag::suspicion_key(p4));
  CHECK(h.empty());
}

PLAG_TEST(top_k_returns_highest_pairs_without_destroying_heap) {
  plag::MaxHeap h;
  for (int i = 1; i <= 10; ++i) {
    h.push(make_pair(0, static_cast<plag::DocumentId>(i), i * 0.1));
  }

  plag::DynArray<plag::PairScore> top3;
  std::size_t got = h.top_k(3, top3);
  CHECK_EQ(got, std::size_t{3});
  CHECK_EQ(h.size(), std::size_t{10});
  CHECK_NEAR(top3[0].coverage, 1.0, 1e-9);
  CHECK_NEAR(top3[1].coverage, 0.9, 1e-9);
  CHECK_NEAR(top3[2].coverage, 0.8, 1e-9);
}
