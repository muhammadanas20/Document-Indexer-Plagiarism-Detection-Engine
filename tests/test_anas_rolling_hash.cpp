// test_anas_rolling_hash.cpp -- the O(1) roll, and the property that makes the
// whole index correct.
// Owner: Anas.  Step 4 (Week 2).
//
// STUB: init/roll are dummies returning 0. The property test below is the one
// that must be in CI (not a scratch file) before Step 4 is called done.
#include <cstdint>
#include <vector>

#include "common/mulmod.hpp"
#include "index/rolling_hash.hpp"
#include "harness.hpp"

namespace {

// Recompute the window from scratch -- the O(k) answer the O(1) roll must match.
std::uint64_t recompute(const std::uint64_t* w, std::size_t k, std::uint64_t b,
                        std::uint64_t p) {
  std::uint64_t h = 0;
  for (std::size_t i = 0; i < k; ++i) {
    h = plag::mulmod(h, b, p);
    h = (h + w[i] % p) % p;
  }
  return h;
}

class Rng {
 public:
  explicit Rng(std::uint64_t seed) : s_(seed | 1ULL) {}
  std::uint64_t next() {
    s_ ^= s_ >> 12;
    s_ ^= s_ << 25;
    s_ ^= s_ >> 27;
    return s_ * 2685821657736338717ULL;
  }
 private:
  std::uint64_t s_;
};

}  // namespace

PLAG_TEST(default_modulus_is_two_pow_61_minus_one) {
  CHECK_EQ(plag::RollingHash::default_modulus(), (std::uint64_t{1} << 61) - 1);
}

PLAG_TEST(rolled_value_equals_recomputed_value_on_100k_random_windows) {
  // THE property test: 100 000 random 64-bit words, slide the window one token
  // at a time, and the rolled value must equal a from-scratch recompute every
  // single time. This is the viva answer.
  const std::uint64_t p = plag::RollingHash::default_modulus();
  const std::uint64_t b = 1000003;
  constexpr std::size_t kK = 15;
  constexpr std::size_t kN = 100000;

  std::vector<std::uint64_t> w(kN);
  Rng rng(20261004);
  for (std::size_t i = 0; i < kN; ++i) w[i] = rng.next();

  plag::RollingHash rh;
  rh.init(w.data(), kK, b, p);

  // STUB: init/roll are dummies, so the value stays 0. When Step 4 lands this
  // becomes the real equivalence loop:
  //   CHECK_EQ(rh.hash(), recompute(&w[i], kK, b, p));
  //   CHECK(rh.hash() < p);
  CHECK_EQ(rh.hash(), std::uint64_t{0});
  PLAG_STUB_NOTE("Step 4 -- roll vs recompute equivalence, 100k windows");

  // Keep the oracle compiled in from day one so Step 4 is a one-line switch.
  CHECK_EQ(recompute(w.data(), kK, b, p), recompute(w.data(), kK, b, p));
}

PLAG_TEST(identical_token_sequences_produce_identical_hash_sequences) {
  // Two different documents with the same token sequence must hash identically
  // -- the index matches on fingerprints, not on files.
  const std::uint64_t p = plag::RollingHash::default_modulus();
  const std::uint64_t b = 1000003;
  const std::uint64_t toks[8] = {7, 8, 9, 10, 11, 12, 13, 14};

  plag::RollingHash a;
  plag::RollingHash b2;
  a.init(toks, 4, b, p);
  b2.init(toks, 4, b, p);
  CHECK_EQ(a.hash(), b2.hash());
}

PLAG_TEST(value_stays_below_the_modulus) {
  // Debug-build assert target: an unsigned underflow here means the modulus was
  // not added before the subtraction.
  const std::uint64_t p = plag::RollingHash::default_modulus();
  plag::RollingHash rh;
  const std::uint64_t toks[4] = {1, 2, 3, 4};
  rh.init(toks, 4, 1000003, p);
  rh.roll(1, 5);
  CHECK(rh.hash() < p);
}
