// Unit tests for RankTree (AVL tree leaderboard)
#include "rank/rank_tree.hpp"
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
  p.matchedTokens = static_cast<std::uint32_t>(cov * 1000.0);
  return p;
}

}  // namespace

PLAG_TEST(empty_tree_has_zero_size_and_height) {
  plag::RankTree t;
  CHECK_EQ(t.size(), std::size_t{0});
  CHECK_EQ(t.height(), std::size_t{0});
}

PLAG_TEST(sorted_insertions_stay_balanced) {
  plag::RankTree t;
  for (int i = 1; i <= 1000; ++i) {
    t.insert(make_pair(0, static_cast<plag::DocumentId>(i), i / 1000.0));
  }

  CHECK_EQ(t.size(), std::size_t{1000});
  // For 1000 nodes, an AVL tree height is at most ~15 (1.44 * log2(1000))
  CHECK(t.height() <= 15);
  CHECK(t.height() >= 10);
}

PLAG_TEST(top_k_walk_is_descending) {
  plag::RankTree t;
  t.insert(make_pair(0, 1, 0.20));
  t.insert(make_pair(0, 2, 0.85));
  t.insert(make_pair(0, 3, 0.40));
  t.insert(make_pair(0, 4, 0.65));
  t.insert(make_pair(0, 5, 0.95));

  plag::DynArray<plag::PairScore> top;
  std::size_t n = t.top_k(3, top);
  CHECK_EQ(n, std::size_t{3});
  CHECK_NEAR(top[0].coverage, 0.95, 1e-9);
  CHECK_NEAR(top[1].coverage, 0.85, 1e-9);
  CHECK_NEAR(top[2].coverage, 0.65, 1e-9);
}
