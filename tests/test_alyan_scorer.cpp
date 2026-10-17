// Unit tests for similarity scoring functions
#include "search/scorer.hpp"
#include "search/verify_mode.hpp"
#include "harness.hpp"

PLAG_TEST(zero_inputs_score_zero) {
  CHECK_NEAR(plag::jaccard(0, 0, 0), 0.0, 1e-9);
  CHECK_NEAR(plag::containment(0, 0), 0.0, 1e-9);
  CHECK_NEAR(plag::coverage(0, 0), 0.0, 1e-9);
}

PLAG_TEST(jaccard_of_identical_sets_is_one) {
  CHECK_NEAR(plag::jaccard(25, 25, 25), 1.0, 1e-9);
  CHECK_NEAR(plag::jaccard(10, 20, 20), 10.0 / 30.0, 1e-9);
}

PLAG_TEST(containment_catches_small_copy_in_large_file) {
  // 20 shared k-grams where the smaller document has 20 k-grams and larger has 500
  plag::PairScore ps = plag::score_pair(0, 1, 20, 20, 500, 20, 1);
  CHECK_NEAR(ps.containment, 1.0, 1e-9);
  CHECK_NEAR(ps.coverage, 1.0, 1e-9);
  CHECK_EQ(ps.label, static_cast<std::uint8_t>(plag::Suspicion::Critical));
}
