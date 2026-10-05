// rolling_hash.hpp -- Rabin-Karp window hash that rolls in O(1).
// Owner: Anas.  src/index/ is Anas's area.
// STUB (W2 / Step 4): dummy bodies return 0. Replace when Step 4 lands.
#ifndef PLAG_SRC_INDEX_ROLLING_HASH_HPP_
#define PLAG_SRC_INDEX_ROLLING_HASH_HPP_

#include <cstddef>
#include <cstdint>

namespace plag {

// H = (t0*b^(k-1) + t1*b^(k-2) + ... + t(k-1)) mod p
// Roll: H' = ((H - out*highPow) * b + in) mod p   -- O(1), not O(k).
// p = 2^61 - 1, base is a random odd number chosen at start-up so nobody can
// craft a collision on purpose.
struct RollingHash {
  std::uint64_t base = 0;
  std::uint64_t mod = 0;
  std::uint64_t highPow = 0;   // base^(k-1) mod p, cached in init()
  std::uint64_t value = 0;

  void init(const std::uint64_t* window, std::size_t k, std::uint64_t b,
            std::uint64_t p);
  void roll(std::uint64_t leaving, std::uint64_t entering);
  std::uint64_t hash() const;

  static std::uint64_t default_modulus();
  static std::uint64_t random_base();
};

}  // namespace plag

#endif  // PLAG_SRC_INDEX_ROLLING_HASH_HPP_
