// Implementation of RollingHash
#include "index/rolling_hash.hpp"

#include "common/mulmod.hpp"

namespace plag {

void RollingHash::init(const std::uint64_t* window, std::size_t k,
                       std::uint64_t b, std::uint64_t p) {
  base = b % p;
  mod = p;
  value = 0;
  if (k == 0 || window == nullptr) {
    highPow = 1;
    return;
  }
  highPow = powmod(base, k - 1, mod);
  for (std::size_t i = 0; i < k; ++i) {
    value = mulmod(value, base, mod);
    value = (value + (window[i] % mod)) % mod;
  }
}

void RollingHash::roll(std::uint64_t leaving, std::uint64_t entering) {
  std::uint64_t term = mulmod(leaving % mod, highPow, mod);
  value = submod(value, term, mod);
  value = mulmod(value, base, mod);
  value = (value + (entering % mod)) % mod;
}

std::uint64_t RollingHash::hash() const {
  return value;
}

std::uint64_t RollingHash::default_modulus() {
  return kMersenne61;
}

std::uint64_t RollingHash::random_base() {
  return 1000003ULL;
}

}  // namespace plag
