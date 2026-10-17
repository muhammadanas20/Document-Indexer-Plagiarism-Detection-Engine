// Unit tests for KMP LPS construction and pattern matching
#include <cstdint>
#include <vector>

#include "search/kmp.hpp"
#include "harness.hpp"

PLAG_TEST(lps_table_matches_expected_values) {
  const std::uint64_t pat[5] = {1, 1, 2, 1, 1};
  plag::DynArray<std::uint32_t> lps;
  plag::build_lps(pat, 5, lps);

  CHECK_EQ(lps.size(), std::size_t{5});
  CHECK_EQ(lps[0], 0u);
  CHECK_EQ(lps[1], 1u);
  CHECK_EQ(lps[2], 0u);
  CHECK_EQ(lps[3], 1u);
  CHECK_EQ(lps[4], 2u);
}

PLAG_TEST(kmp_finds_overlapping_occurrences) {
  const std::uint64_t text[7] = {1, 2, 1, 2, 1, 2, 1};
  const std::uint64_t pat[3] = {1, 2, 1};
  plag::DynArray<std::uint32_t> hits;

  std::size_t count = plag::kmp_find_all(text, 7, pat, 3, hits);
  CHECK_EQ(count, std::size_t{3});
  CHECK_EQ(hits[0], 0u);
  CHECK_EQ(hits[1], 2u);
  CHECK_EQ(hits[2], 4u);
}

PLAG_TEST(kmp_matches_naive_search) {
  const std::uint64_t text[12] = {5, 6, 7, 5, 6, 8, 5, 6, 7, 5, 6, 7};
  const std::uint64_t pat[3] = {5, 6, 7};

  plag::DynArray<std::uint32_t> kmp_hits;
  plag::kmp_find_all(text, 12, pat, 3, kmp_hits);

  std::vector<std::uint32_t> naive_hits;
  for (std::size_t i = 0; i + 3 <= 12; ++i) {
    if (text[i] == pat[0] && text[i + 1] == pat[1] && text[i + 2] == pat[2]) {
      naive_hits.push_back(static_cast<std::uint32_t>(i));
    }
  }

  CHECK_EQ(kmp_hits.size(), naive_hits.size());
  for (std::size_t i = 0; i < naive_hits.size(); ++i) {
    CHECK_EQ(kmp_hits[i], naive_hits[i]);
  }
}
