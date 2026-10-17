// Unit tests for RollingHash
#include <cstdint>
#include <vector>

#include "common/mulmod.hpp"
#include "index/rolling_hash.hpp"
#include "harness.hpp"

namespace {

std::uint64_t recompute_window(const std::uint64_t* w, std::size_t k,
                               std::uint64_t b, std::uint64_t p) {
  std::uint64_t h = 0;
  for (std::size_t i = 0; i < k; ++i) {
    h = plag::mulmod(h, b, p);
    h = (h + (w[i] % p)) % p;
  }
  return h;
}

}  // namespace

PLAG_TEST(default_modulus_is_mersenne_61) {
  CHECK_EQ(plag::RollingHash::default_modulus(), (std::uint64_t{1} << 61) - 1);
}

PLAG_TEST(rolled_hash_matches_recomputed_hash) {
  const std::uint64_t p = plag::RollingHash::default_modulus();
  const std::uint64_t b = 1000003ULL;
  constexpr std::size_t kWindow = 10;
  constexpr std::size_t kTotal = 2000;

  std::vector<std::uint64_t> tokens(kTotal);
  for (std::size_t i = 0; i < kTotal; ++i) {
    tokens[i] = (i * 1315423911ULL) + 17ULL;
  }

  plag::RollingHash rh;
  rh.init(tokens.data(), kWindow, b, p);
  CHECK_EQ(rh.hash(), recompute_window(tokens.data(), kWindow, b, p));

  for (std::size_t i = 0; i + kWindow < kTotal; ++i) {
    rh.roll(tokens[i], tokens[i + kWindow]);
    std::uint64_t expected =
        recompute_window(&tokens[i + 1], kWindow, b, p);
    CHECK_EQ(rh.hash(), expected);
    CHECK(rh.hash() < p);
  }
}

PLAG_TEST(identical_sequences_produce_equal_hashes) {
  const std::uint64_t p = plag::RollingHash::default_modulus();
  const std::uint64_t b = 1000003ULL;
  const std::uint64_t toks[6] = {10, 20, 30, 40, 50, 60};

  plag::RollingHash r1;
  plag::RollingHash r2;
  r1.init(toks, 4, b, p);
  r2.init(toks, 4, b, p);
  CHECK_EQ(r1.hash(), r2.hash());

  r1.roll(toks[0], toks[4]);
  r2.roll(toks[0], toks[4]);
  CHECK_EQ(r1.hash(), r2.hash());
}
